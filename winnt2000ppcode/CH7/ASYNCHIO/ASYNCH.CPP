/**-----------------------------------------------------------
**  ASYNCH.CPP :  Simple Win32 File I/O Class methods.
**  Environment:  Windows NT, MS Visual C/C++ (32-bit)
**---------------------------------------------------------**/
#include <windows.h>
#include "asynch.h"

/**--------------------------------------------------------**/
CFileIO::CFileIO(char *pFilename, DWORD dwAccess, DWORD dwCreate,
    DWORD dwAttribute, DWORD *dwStatus)
{
    hFile = CreateFile(pFilename, dwAccess, 0, NULL, 
        dwCreate, dwAttribute, NULL);
    if (hFile == INVALID_HANDLE_VALUE) *dwStatus = FALSE;
    else *dwStatus = TRUE;

    if (dwAttribute & FILE_FLAG_OVERLAPPED) bAsynch = TRUE;
    else bAsynch = FALSE;

    strcpy(reopenName, pFilename);
    reopenAccess = dwAccess;
    reopenCreate = dwCreate;
    reopenAttrib = dwAttribute;

    olFile.hEvent = NULL;
    olFile.Offset = olFile.OffsetHigh = 0;
} // CFileIO

/**--------------------------------------------------------**/
CFileIO::~CFileIO()
{
    if (hFile != NULL) {
        CloseHandle(hFile);
        hFile = NULL;
    }
} // CFileIO::~CFileIO

/**--------------------------------------------------------**/
DWORD CFileIO::Read(BYTE *pBuffer, DWORD dwSize)
{
    DWORD dwBytes;

    if (ReadFile(hFile, pBuffer, dwSize, &dwBytes, NULL))
        return dwBytes;
    else return (DWORD)0L;
} // CFileIO::Read

/**--------------------------------------------------------**/
DWORD CFileIO::Write(BYTE *pBuffer, DWORD dwSize)
{
    DWORD dwBytes;

    if (WriteFile(hFile, pBuffer, dwSize, &dwBytes, NULL))
        return dwBytes;
    else return (DWORD)0L;
} // CFileIO::Write

/**--------------------------------------------------------**/
DWORD CFileIO::ReadWithAbandon(BYTE *pBuffer, DWORD dwSize)
{
    DWORD dwBytes;

    // Sorry, you didn't open it for asynchronous operation.
    if (!bAsynch) return Read(pBuffer, dwSize);

    IOComplete(TRUE, &dwBytes);
    olFile.Offset += dwBytes;
    return ReadFile(hFile, pBuffer, dwSize, &dwPend, &olFile);
} // CFileIO::ReadWithAbandon

/**--------------------------------------------------------**/
DWORD CFileIO::WriteWithAbandon(BYTE *pBuffer, DWORD dwSize)
{
    DWORD dwBytes;

    // Sorry, you didn't open it for asynchronous operation.
    if (!bAsynch) return Write(pBuffer, dwSize);

    IOComplete(TRUE, &dwBytes);
    olFile.Offset += dwBytes;
    return WriteFile(hFile, pBuffer, dwSize, &dwPend, &olFile);
} // CFileIO::WriteWithAbandon

/**--------------------------------------------------------**/
BOOL CFileIO::Close(void)
{
    BOOL status = CloseHandle(hFile);
    hFile = NULL;
    return status;
} // CFileIO::Close

/**--------------------------------------------------------**/
BOOL CFileIO::Reopen(void)
{
    hFile = CreateFile(reopenName, reopenAccess, 0, NULL, 
        reopenCreate, reopenAttrib, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;
    else return TRUE;
} // CFileIO::Reopen

/**--------------------------------------------------------**/
DWORD CFileIO::Seek(LONG lOffset, DWORD dwMethod)
{
    return SetFilePointer(hFile, lOffset, 0, dwMethod); 
} // CFileIO::Seek

/**--------------------------------------------------------**/
HANDLE CFileIO::GetHandle(void)
{
    return hFile;
} // CFileIO::GetHandle

/**--------------------------------------------------------**/
BOOL CFileIO::IOComplete(BOOL bWait, DWORD *dwBytes)
{
    // Don't wait for completion, just return current status
    if (!bWait) {
        if (!GetOverlappedResult(hFile, &olFile, dwBytes, FALSE)
            && (GetLastError() == ERROR_IO_PENDING)) return FALSE;
        else return TRUE;
    }
    // Wait for the file I/O operation to complete
    else {
        if (GetOverlappedResult(hFile, &olFile, dwBytes, FALSE))
            GetOverlappedResult(hFile, &olFile, dwBytes, TRUE);
        return TRUE;
    }
} // CFileIO::IOComplete 
// End of File 
