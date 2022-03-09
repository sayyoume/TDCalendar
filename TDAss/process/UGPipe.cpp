
#include "UGPipe.h"

namespace Oasgames{
	namespace core{

#define INSTANCES 1 

#define BUFSIZE 4096
		typedef struct
		{
			OVERLAPPED oOverlap;
			HANDLE hPipeInst;
			TCHAR chRequest[BUFSIZE];
			DWORD cbRead;
			TCHAR chReply[BUFSIZE];
			DWORD cbToWrite;
			DWORD dwState;
			BOOL fPendingIO;
		} PIPEINST, *LPPIPEINST;

		PIPEINST		Pipe[INSTANCES];
		HANDLE			hEvents[INSTANCES];

#define CONNECTING_STATE 0 
#define READING_STATE 1 
#define WRITING_STATE 2 

		UGPipeServer::UGPipeServer()
		{
			m_bStop = true;
		}

		UGPipeServer::~UGPipeServer()
		{
			stop(); 
		}

#define MCE_INHERITHANDLE		0x00000001
#define MCE_NORMALACCESS		0x00000002
#define FLAG_ISSET(flag,value)					(((flag) & (value))==(value))

		BOOL CreateSecurityAttributes(PSECURITY_ATTRIBUTES psa, DWORD dwFlag)
		{
			if (psa == NULL)
				return FALSE;

			psa->lpSecurityDescriptor = NULL;

			PSECURITY_DESCRIPTOR pSD = NULL;

			if (FLAG_ISSET(dwFlag, MCE_NORMALACCESS))
			{
				pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
				if (pSD == NULL)
					goto ENDPOS;

				if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
					goto ENDPOS;

				if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE))
					goto ENDPOS;

				psa->lpSecurityDescriptor = pSD;
			}
			else
				psa->lpSecurityDescriptor = NULL;

			psa->bInheritHandle = FLAG_ISSET(dwFlag, MCE_INHERITHANDLE);
			psa->nLength = sizeof(SECURITY_ATTRIBUTES);

			return TRUE;

		ENDPOS:

			return FALSE;
		}

		BOOL FreeSecurityAttributes(PSECURITY_ATTRIBUTES psa)
		{
			if (psa != NULL && psa->lpSecurityDescriptor != NULL)
			{
				LocalFree(psa->lpSecurityDescriptor);
				psa->lpSecurityDescriptor = NULL;
			}

			return TRUE;
		}

		bool UGPipeServer::listen(const std::wstring& sName, const std::wstring& sGuid, const std::function<int(const std::wstring& sRecvData, std::wstring& sAnswer)>& func)
		{
			if (!m_bStop)
				return false;

			if (func._Empty())
				return false;

			m_bStop = false;

			wchar_t pipeBuff[256] = { 0 };
			swprintf_s(pipeBuff, L"\\\\.\\pipe\\J_%s_%s", sName.c_str(), sGuid.c_str());
			sPipeName = pipeBuff;

			m_future = std::async(std::launch::async, [&,func]
			{
				SECURITY_ATTRIBUTES sa = { 0 };
				if (!CreateSecurityAttributes(&sa, MCE_NORMALACCESS))
					return;

#define BUFSIZE 4096
				while (!m_bStop)
				{
					HANDLE hPipe = CreateNamedPipe(sPipeName.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
						PIPE_UNLIMITED_INSTANCES, BUFSIZE, BUFSIZE, 5000, &sa);
					
					if (hPipe == INVALID_HANDLE_VALUE)
					{
						Sleep(100);
						continue;
					}

					BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
					if (fConnected && !m_bStop)
					{
						try
						{
							while (!m_bStop)
							{
								wchar_t recvBuff[BUFSIZE] = { 0 };
								DWORD cbBytesRead = 0;
								BOOL fSuccess = ReadFile(hPipe, recvBuff, BUFSIZE*sizeof(TCHAR), &cbBytesRead, NULL);

								if (!fSuccess || cbBytesRead == 0)
									break;

								std::wstring sAnswer = L"";
								int iWait = func(std::wstring(recvBuff), sAnswer);
								if (iWait > 0)
								{
									DWORD dwTimer = GetTickCount();

									while (!m_bStop)
									{
										if (!sAnswer.empty())
											break;

										if ((GetTickCount() - dwTimer) >= iWait)
											break;

										Sleep(10);
									}
								}

								if (!m_bStop)
								{
									if (!sAnswer.empty())
									{
										DWORD cbWritten = 0;
										DWORD dwWriteLen = (sAnswer.length() + 1)*sizeof(TCHAR);

										fSuccess = WriteFile(hPipe, sAnswer.c_str(), dwWriteLen, &cbWritten, NULL);

										if (!fSuccess || dwWriteLen != cbWritten)
											break;
									}
								}
								else
								{
									break;
								}
							}

							FlushFileBuffers(hPipe);
							DisconnectNamedPipe(hPipe);
							CloseHandle(hPipe);
						}
						catch (...){}
					}
					else
					{
						CloseHandle(hPipe);
					}
				}

				try
				{
					FreeSecurityAttributes(&sa);
				}
				catch (...){}
				
			});

			return true;
		}

		void UGPipeServer::stop()
		{
			if (m_bStop)
				return;
			m_bStop = true;
			
			DeleteFile(sPipeName.c_str());

			int iError = GetLastError();
			if (iError == 0)
			{
				if (m_future.valid())
					m_future.get();
			}
		}

		BOOL UGPipeServer::ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
		{
			BOOL fConnected, fPendingIO = FALSE;
			fConnected = ConnectNamedPipe(hPipe, lpo);
			if (fConnected)
			{
				printf("ConnectNamedPipe failed with %d.\n", GetLastError());
				return 0;
			}

			switch (GetLastError())
			{
			case ERROR_IO_PENDING:
				fPendingIO = TRUE;
				break;
			case ERROR_PIPE_CONNECTED:
				if (SetEvent(lpo->hEvent))
					break;

			default:
			{
				printf("ConnectNamedPipe failed with %d.\n", GetLastError());
				return 0;
			}
			}

			return fPendingIO;
		}

		bool UGPipeServer::listen2(const std::wstring& sName, const std::wstring& sGuid, const std::function<int(const std::wstring& sRecvData)>& func)
		{
			if (!m_bStop)
				return false;

			if (func._Empty())
				return false;

			m_bStop = false;

			wchar_t pipeBuff[256] = { 0 };
			swprintf_s(pipeBuff, L"\\\\.\\pipe\\J_%s_%s", sName.c_str(), sGuid.c_str());
			sPipeName = pipeBuff;


			m_future = std::async(std::launch::async, [&, func]
			{
			
				DWORD i, dwWait, cbRet, dwErr;
				BOOL fSuccess;

				for (i = 0; i < INSTANCES; i++)
				{
					hEvents[i] = CreateEvent(
						NULL,    // default security attribute 
						TRUE,    // manual-reset event 
						TRUE,    // initial state = signaled 
						NULL);   // unnamed event object 

					if (hEvents[i] == NULL)
					{
						printf("CreateEvent failed with %d.\n", GetLastError());
						return;
					}

					Pipe[i].oOverlap.hEvent = hEvents[i];

					SECURITY_ATTRIBUTES sa = { 0 };
					if (!CreateSecurityAttributes(&sa, MCE_NORMALACCESS))
						return;

					wchar_t buff[256] = { 0 };

					Pipe[i].hPipeInst = CreateNamedPipe(
						sPipeName.c_str(),					 // pipe name 
						PIPE_ACCESS_DUPLEX |     // read/write access 
						FILE_FLAG_OVERLAPPED,    // overlapped mode 
						PIPE_TYPE_MESSAGE |      // message-type pipe 
						PIPE_READMODE_MESSAGE |  // message-read mode 
						PIPE_WAIT,               // blocking mode 
						INSTANCES,               // number of instances 
						BUFSIZE*sizeof(TCHAR),   // output buffer size 
						BUFSIZE*sizeof(TCHAR),   // input buffer size 
						5000,					// client time-out 
						&sa);                   // default security attributes 

					if (Pipe[i].hPipeInst == INVALID_HANDLE_VALUE)
					{
						return;
					}

					FreeSecurityAttributes(&sa);

					Pipe[i].fPendingIO = ConnectToNewClient(
						Pipe[i].hPipeInst,
						&Pipe[i].oOverlap);

					Pipe[i].dwState = Pipe[i].fPendingIO ?
					CONNECTING_STATE : // still connecting 
									   READING_STATE;     // ready to read 
				}

				bool bExit = false;
				while (!bExit)
				{
					dwWait = WaitForMultipleObjects(
						INSTANCES,    // number of event objects 
						hEvents,      // array of event objects 
						FALSE,        // does not wait for all 
						INFINITE);    // waits indefinitely 

					i = dwWait - WAIT_OBJECT_0;  // determines which pipe 
					if (i < 0 || i >(INSTANCES - 1))
					{
						printf("Index out of range.\n");
						return;
					}

					if (m_bStop)
					{
						DisconnectNamedPipe(Pipe[i].hPipeInst);
						Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
						Pipe[i].dwState = Pipe[i].fPendingIO ? CONNECTING_STATE : READING_STATE;
						break;
					}

					if (Pipe[i].fPendingIO)
					{
						fSuccess = GetOverlappedResult(
							Pipe[i].hPipeInst, // handle to pipe 
							&Pipe[i].oOverlap, // OVERLAPPED structure 
							&cbRet,            // bytes transferred 
							FALSE);            // do not wait 

						switch (Pipe[i].dwState)
						{
							// Pending connect operation 
						case CONNECTING_STATE:
							if (!fSuccess)
							{
								printf("Error %d.\n", GetLastError());
								return;
							}
							Pipe[i].dwState = READING_STATE;
							break;

							// Pending read operation 
						case READING_STATE:
							if (!fSuccess || cbRet == 0)
							{
								DisconnectNamedPipe(Pipe[i].hPipeInst);
								Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
								Pipe[i].dwState = Pipe[i].fPendingIO ? CONNECTING_STATE : READING_STATE;
								continue;
							}
							Pipe[i].cbRead = cbRet;
							Pipe[i].dwState = WRITING_STATE;
							break;

							// Pending write operation 
						case WRITING_STATE:
							if (!fSuccess || cbRet != Pipe[i].cbToWrite)
							{
								DisconnectNamedPipe(Pipe[i].hPipeInst);
								Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
								Pipe[i].dwState = Pipe[i].fPendingIO ? CONNECTING_STATE : READING_STATE;
								continue;
							}
							Pipe[i].dwState = READING_STATE;
							break;

						default:
						{
							printf("Invalid pipe state.\n");
							return;
						}
						}
					}

					switch (Pipe[i].dwState)
					{
					case READING_STATE:

						memset(Pipe[i].chRequest, 0, BUFSIZE*sizeof(TCHAR));
						fSuccess = ReadFile(
							Pipe[i].hPipeInst,
							Pipe[i].chRequest,
							BUFSIZE*sizeof(TCHAR),
							&Pipe[i].cbRead,
							&Pipe[i].oOverlap);

						if (fSuccess && Pipe[i].cbRead != 0)
						{
							Pipe[i].fPendingIO = FALSE;
							Pipe[i].dwState = WRITING_STATE;
							continue;
						}

						dwErr = GetLastError();
						if (!fSuccess && (dwErr == ERROR_IO_PENDING))
						{
							Pipe[i].fPendingIO = TRUE;
							continue;
						}
						
						DisconnectNamedPipe(Pipe[i].hPipeInst);
						Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
						Pipe[i].dwState = Pipe[i].fPendingIO ? CONNECTING_STATE : READING_STATE;

						break;

					case WRITING_STATE:
						{
							func(std::wstring(Pipe[i].chRequest));
						}

						fSuccess = WriteFile(
							Pipe[i].hPipeInst,
							Pipe[i].chReply,
							Pipe[i].cbToWrite,
							&cbRet,
							&Pipe[i].oOverlap);

						if (fSuccess && cbRet == Pipe[i].cbToWrite)
						{
							Pipe[i].fPendingIO = FALSE;
							Pipe[i].dwState = READING_STATE;
							continue;
						}

						dwErr = GetLastError();
						if (!fSuccess && (dwErr == ERROR_IO_PENDING))
						{
							Pipe[i].fPendingIO = TRUE;
							continue;
						}

						DisconnectNamedPipe(Pipe[i].hPipeInst);
						Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
						Pipe[i].dwState = Pipe[i].fPendingIO ? CONNECTING_STATE : READING_STATE;

						break;
					default:
						{
							printf("Invalid pipe state.\n");
							return;
						}
					}
				}
			
			});


			return true;
		}

		bool sendPipeData(const std::wstring& sData, const std::wstring& sPipeName, const std::wstring& sGuid,
			const std::function<void(const std::wstring& sAnswer, bool bAnswer)>& func, int iWaitTimer)
		{
			HANDLE hPipe;
			TCHAR  chBuf[BUFSIZE];
			BOOL   fSuccess = FALSE;
			DWORD  cbRead, cbToWrite, cbWritten, dwMode;

			wchar_t buff[256] = { 0 };
			swprintf_s(buff, L"\\\\.\\pipe\\J_%s_%s", sPipeName.c_str(), sGuid.c_str());

			SECURITY_ATTRIBUTES sa = { 0 };
			if (!CreateSecurityAttributes(&sa, MCE_NORMALACCESS))
				return false;

			while (1)
			{
				hPipe = CreateFile(
					buff,   // pipe name 
					GENERIC_READ |  // read and write access 
					GENERIC_WRITE,
					0,              // no sharing 
					&sa,           // default security attributes
					OPEN_EXISTING,  // opens existing pipe 
					FILE_FLAG_OVERLAPPED,   // default attributes 
					NULL);          // no template file 

				if (hPipe != INVALID_HANDLE_VALUE)
					break;
				if (GetLastError() != ERROR_PIPE_BUSY)
				{
					return false;
				}

				if (!WaitNamedPipe(buff, 5000))
				{
					return false;
				}
			}

			FreeSecurityAttributes(&sa);

			dwMode = PIPE_READMODE_MESSAGE;
			fSuccess = SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);
			if (!fSuccess)
				return false;

			cbToWrite = (sData.length() + 1)*sizeof(TCHAR);

			fSuccess = WriteFile(hPipe, sData.c_str(), cbToWrite, &cbWritten, NULL);

			if (!func._Empty())
			{
				if (!fSuccess)
				{
					CloseHandle(hPipe);
					func(L"", false);
				}
				else
				{
					DWORD dwTimer = GetTickCount();

					BOOL bReadSuccess = FALSE;
					BOOL bExit = FALSE;

					std::future<void> m_future = std::async(std::launch::async, [&, hPipe]()
					{
						bReadSuccess = ReadFile(hPipe, chBuf, BUFSIZE*sizeof(TCHAR), &cbRead, NULL);
						bExit = TRUE;
					});

					while (!bExit)
					{
						if (bExit)
							break;

						if ((GetTickCount() - dwTimer) >= iWaitTimer)
							break;

						Sleep(10);
					}

					if (bReadSuccess)
					{
						func(std::wstring(chBuf), true);
					}
					else
					{ 
						func(L"", false);

						DeleteFile(sPipeName.c_str());

						/*if (m_future.valid())
							m_future.get();*/
					}
				}
			}

			if (!fSuccess)
				return false;

			CloseHandle(hPipe);

			return true;
		}

		bool sendPipeData(const std::wstring& sData, const std::wstring& sPipeName, const std::wstring& sGuid)
		{
			std::function<void(const std::wstring& sAnswer, bool bAnswer)> func;
			return sendPipeData(sData, sPipeName, sGuid, func, 0);
		}
	}
}