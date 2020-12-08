// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#include "FileTransferMainDialog.h"

#include "util/CommonHeader.h"
#include "util/winhdr.h"
#include "NewFolderDialog.h"
#include "FileRenameDialog.h"

#include "file-lib/File.h"

#include "resource.h"
#include <stdio.h>

FileTransferMainDialog::FileTransferMainDialog(FileTransferCore *core)
: FileTransferInterface(core)
{
  setResourceId(ftclient_mainDialog);

  m_lastSentFileListPath.setString(_T(""));
  m_lastReceivedFileListPath.setString(_T(""));

  m_fakeMoveUpFolder = new FileInfo(0, 0, FileInfo::DIRECTORY, _T(".."));
}

FileTransferMainDialog::~FileTransferMainDialog()
{
  delete m_fakeMoveUpFolder;
}

void FileTransferMainDialog::setProgress(double progress)
{
  WORD pt = 1000;
  WORD pc = static_cast<WORD>(progress * pt);

  m_copyProgressBar.setPos(pc);
}

int FileTransferMainDialog::onFtTargetFileExists(FileInfo *sourceFileInfo,
                                                 FileInfo *targetFileInfo,
                                                 const TCHAR *pathToTargetFile)
{
  m_fileExistDialog.setFilesInfo(targetFileInfo,
                                 sourceFileInfo,
                                 pathToTargetFile);

  int reasonOfDialog = m_fileExistDialog.showModal();
  switch (reasonOfDialog) {
  case FileExistDialog::SKIP_RESULT:
    return CopyFileEventListener::TFE_SKIP;
  case FileExistDialog::APPEND_RESULT:
    return CopyFileEventListener::TFE_APPEND;
  case FileExistDialog::CANCEL_RESULT:
    onCancelOperationButtonClick();
    return CopyFileEventListener::TFE_CANCEL;
  } // switch

  return CopyFileEventListener::TFE_OVERWRITE;
}

BOOL FileTransferMainDialog::onInitDialog()
{
  m_isClosing = false;

  initControls();

  tryListRemoteFolder(_T("/"));
  tryListLocalFolder(_T(""));

  return TRUE;
}

BOOL FileTransferMainDialog::onNotify(UINT controlID, LPARAM data)
{
  LPNMHDR nmhdr = (LPNMHDR)data;
  switch (controlID) {
  case IDC_REMOTE_FILE_LIST:
    switch (nmhdr->code) {
    case NM_DBLCLK:
      onRemoteListViewDoubleClick();
      break;
    case LVN_KEYDOWN:
      {
        LPNMLVKEYDOWN nmlvkd = (LPNMLVKEYDOWN)data;
        onRemoteListViewKeyDown(nmlvkd->wVKey);
      }
      break;
    case LVN_COLUMNCLICK:
      {
        NMLISTVIEW *lpdi = reinterpret_cast<NMLISTVIEW *>(data);
        m_remoteFileListView.sort(lpdi->iSubItem);
      }
      break;
    } // switch notification code

    //
    // FIXME: Not better way to call this method at every notification
    // for list view control, but windows have no notification for list view
    // selection changed event. So for now, i didn't found better solution.
    //

    checkRemoteListViewSelection();
    break;
  case IDC_LOCAL_FILE_LIST:
    switch (nmhdr->code) {
    case NM_DBLCLK:
      onLocalListViewDoubleClick();
      break;
    case LVN_KEYDOWN:
      {
        LPNMLVKEYDOWN nmlvkd = (LPNMLVKEYDOWN)data;
        onLocalListViewKeyDown(nmlvkd->wVKey);
      }
      break;
    case LVN_COLUMNCLICK:
      {
        NMLISTVIEW *lpdi = reinterpret_cast<NMLISTVIEW *>(data);
        m_localFileListView.sort(lpdi->iSubItem);
      }
      break;
    } // switch notification code

    //
    // FIXME: Not better way to call this method at every notification
    // for list view control, but windows have no notification for list view
    // selection changed event. So for now, i didn't found better solution.
    //

    checkLocalListViewSelection();
    break;
  } // switch controls
  return TRUE;
}

BOOL FileTransferMainDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDCANCEL:
    onCancelButtonClick();
    break;
  case IDC_CANCEL_BUTTON:
    onCancelOperationButtonClick();
    break;
  case IDC_RENAME_REMOTE_BUTTON:
    onRenameRemoteButtonClick();
    break;
  case IDC_MKDIR_REMOTE_BUTTON:
    onMkDirRemoteButtonClick();
    break;
  case IDC_REMOVE_REMOTE_BUTTON:
    onRemoveRemoteButtonClick();
    break;
  case IDC_REFRESH_REMOTE_BUTTON:
    onRefreshRemoteButtonClick();
    break;
  case IDC_RENAME_LOCAL_BUTTON:
    onRenameLocalButtonClick();
    break;
  case IDC_MKDIR_LOCAL_BUTTON:
    onMkDirLocalButtonClick();
    break;
  case IDC_REMOVE_LOCAL_BUTTON:
    onRemoveLocalButtonClick();
    break;
  case IDC_REFRESH_LOCAL_BUTTON:
    onRefreshLocalButtonClick();
    break;
  case IDC_UPLOAD_BUTTON:
    onUploadButtonClick();
    break;
  case IDC_DOWNLOAD_BUTTON:
    onDownloadButtonClick();
    break;
  }
  return TRUE;
}

BOOL FileTransferMainDialog::onDestroy()
{
  return TRUE;
}

void FileTransferMainDialog::onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_OPERATION_FINISHED:
    m_ftCore->onOperationFinished();

    if (!m_isClosing) {
      int state = static_cast<int>(wParam);
      int result = static_cast<int>(lParam);
      m_ftCore->onUpdateState(state, result);

      setProgress(0.0);
      enableControls(true);
      break;
    } else { // If window is closing we can it only if operation finished
      kill(0);
      return;
    } 
  } // switch
} // void

bool FileTransferMainDialog::tryClose()
{
  if (m_ftCore->isNothingState()) {
    // No operation is executing - close dialog
    kill(IDCANCEL);
    return true;
  }
  if (MessageBox(m_ctrlThis.getWindow(),
                 _T("Do you want to close file transfers and terminate current operation?"),
                 _T("TightVNC File Transfers"),
                 MB_YESNO | MB_ICONQUESTION) == IDYES) {
    // Set flag
    m_isClosing = true;
    // Terminate current operation
    m_ftCore->terminateCurrentOperation();
    return true;
  } // if result is not "yes"
  return false;
}

void FileTransferMainDialog::onCancelButtonClick()
{
  tryClose();
}

void FileTransferMainDialog::onCancelOperationButtonClick()
{
  if (!m_ftCore->isNothingState()) {
  // Logging
    StringStorage message(_T("Operation have been canceled by user"));
    insertMessageIntoComboBox(message.getString());

    // Terminate current operation
    m_ftCore->terminateCurrentOperation();
    // Disable "Cancel" button while waiting for a moment
    // when operation will finishe execution.
    m_cancelButton.setEnabled(false);
  }
}

void FileTransferMainDialog::onRenameRemoteButtonClick()
{
  FileInfo *fileInfo = m_remoteFileListView.getSelectedFileInfo();

  if (fileInfo == NULL) {
    MessageBox(m_ctrlThis.getWindow(),
               _T("No file selected."),
               _T("Rename File"), MB_OK | MB_ICONWARNING);
    return ;
  }

  FileRenameDialog renameDialog(&m_ctrlThis);
  renameDialog.setFileName(fileInfo->getFileName());

  if (renameDialog.showModal() == IDOK) {
    StringStorage remoteFolder;
    getPathToCurrentRemoteFolder(&remoteFolder);

    StringStorage oldName(fileInfo->getFileName());

    StringStorage newName;
    renameDialog.getFileName(&newName);

    m_ftCore->remoteFileRenameOperation(FileInfo(0, 0, FileInfo::DIRECTORY, oldName.getString()),
                                        FileInfo(0, 0, FileInfo::DIRECTORY, newName.getString()),
                                        remoteFolder.getString());
  }
}

void FileTransferMainDialog::onMkDirRemoteButtonClick()
{
  NewFolderDialog folderDialog(&m_ctrlThis);
  if (folderDialog.showModal() == IDOK) {
    StringStorage remoteFolder;
    m_remoteCurFolderTextBox.getText(&remoteFolder);

    StringStorage fileName;
    folderDialog.getFileName(&fileName);


    m_ftCore->remoteFolderCreateOperation(FileInfo(0, 0,
                                                   FileInfo::DIRECTORY,
                                                   fileName.getString()),
                                          remoteFolder.getString());
  }
}

void FileTransferMainDialog::onRemoveRemoteButtonClick()
{
  unsigned int siCount = m_remoteFileListView.getSelectedItemsCount();

  if (siCount == 0) {
    MessageBox(m_ctrlThis.getWindow(),
               _T("No files selected."),
               _T("Delete Files"), MB_OK | MB_ICONWARNING);
    return ;
  }

  int *indexes = new int[siCount];
  FileInfo *filesInfo = new FileInfo[siCount];

  m_remoteFileListView.getSelectedItemsIndexes(indexes);
  for (unsigned int i = 0; i < siCount; i++) {
    FileInfo *fileInfo = reinterpret_cast<FileInfo *>(m_remoteFileListView.getItemData(indexes[i]));
    filesInfo[i] = *fileInfo;
  }

  if (MessageBox(m_ctrlThis.getWindow(),
                 _T("Do you wish to delete the selected files?"),
                 _T("Delete Files"),
                 MB_YESNO | MB_ICONQUESTION) != IDYES) {
    delete[] indexes;
    delete[] filesInfo;
    return ;
  }

  StringStorage remoteFolder;
  m_remoteCurFolderTextBox.getText(&remoteFolder);

  m_ftCore->remoteFilesDeleteOperation(filesInfo, siCount,
                                       remoteFolder.getString());
  delete[] indexes;
  delete[] filesInfo;
}

void FileTransferMainDialog::onRefreshRemoteButtonClick()
{
  refreshRemoteFileList();
}

void FileTransferMainDialog::onRenameLocalButtonClick()
{
  FileInfo *fileInfo = m_localFileListView.getSelectedFileInfo();

  if (fileInfo == NULL) {
    MessageBox(m_ctrlThis.getWindow(),
               _T("No file selected."),
               _T("Rename File"), MB_OK | MB_ICONWARNING);
    return ;
  }

  FileRenameDialog renameDialog(&m_ctrlThis);
  renameDialog.setFileName(fileInfo->getFileName());

  if (renameDialog.showModal() == IDOK) {
    StringStorage localFolder;
    getPathToCurrentLocalFolder(&localFolder);

    StringStorage oldName;
    StringStorage newName;

    oldName.setString(fileInfo->getFileName());
    renameDialog.getFileName(&newName);

    StringStorage pathToOldFile(localFolder.getString());
    StringStorage pathToNewFile(localFolder.getString());

    if (!localFolder.endsWith('\\')) {
      pathToOldFile.appendString(_T("\\"));
      pathToNewFile.appendString(_T("\\"));
    }

    pathToOldFile.appendString(oldName.getString());
    pathToNewFile.appendString(newName.getString());

    //
    // Logging
    //

    StringStorage message;

    message.format(_T("Renaming local file '%s' to '%s'"),
                   pathToOldFile.getString(), pathToNewFile.getString());

    insertMessageIntoComboBox(message.getString());

     // Rename local file
    File oldFile(pathToOldFile.getString());

    if (!oldFile.renameTo(pathToNewFile.getString())) {
      message.format(_T("Error: failed to rename local '%s' file"),
                     pathToOldFile.getString());

      insertMessageIntoComboBox(message.getString());
    }

    refreshLocalFileList();
  } // if dialog result is ok
} // void

void FileTransferMainDialog::onMkDirLocalButtonClick()
{
  StringStorage pathToFile;

  getPathToCurrentLocalFolder(&pathToFile);

  // Not allow user to create folders in our "fake" root folder
  if (pathToFile.isEmpty()) {
    MessageBox(m_ctrlThis.getWindow(),
               _T("It's not allowed to create new folder here."),
               _T("New Folder"), MB_OK | MB_ICONWARNING);
  }

  NewFolderDialog folderDialog(&m_ctrlThis);

  if (folderDialog.showModal() == IDOK) {
    StringStorage fileName;
    folderDialog.getFileName(&fileName);

    if (!pathToFile.endsWith(_T('\\'))) {
      pathToFile.appendString(_T("\\"));
    }
    pathToFile.appendString(fileName.getString());

    // Logging
    StringStorage message;

    message.format(_T("Creating local folder '%s'"), pathToFile.getString());

    insertMessageIntoComboBox(message.getString());

    // File system object
    File file(pathToFile.getString());

    // Failed to create local folder
    if (pathToFile.isEmpty() || !file.mkdir()) {
      message.format(_T("Error: failed to create local folder '%s'"),
                     pathToFile.getString());

      insertMessageIntoComboBox(message.getString());
    }

    refreshLocalFileList();
  } // if dialog result is ok
} // void

void FileTransferMainDialog::onRemoveLocalButtonClick()
{
  unsigned int siCount = m_localFileListView.getSelectedItemsCount();

  if (siCount == 0) {
    MessageBox(m_ctrlThis.getWindow(),
               _T("No files selected."),
               _T("Delete Files"), MB_OK | MB_ICONWARNING);
    return ;
  }

  int *indexes = new int[siCount];
  FileInfo *filesInfo = new FileInfo[siCount];

  m_localFileListView.getSelectedItemsIndexes(indexes);
  for (unsigned int i = 0; i < siCount; i++) {
    FileInfo *fileInfo = reinterpret_cast<FileInfo *>(m_localFileListView.getItemData(indexes[i]));
    filesInfo[i] = *fileInfo;
  }

  if (MessageBox(m_ctrlThis.getWindow(),
                 _T("Do you wish to delete the selected files?"),
                 _T("Delete Files"),
                 MB_YESNO | MB_ICONQUESTION) != IDYES) {
    delete[] indexes;
    delete[] filesInfo;
    return ;
  }

  StringStorage localFolder;
  getPathToCurrentLocalFolder(&localFolder);

  m_ftCore->localFilesDeleteOperation(filesInfo, siCount,
                                      localFolder.getString());

  delete[] indexes;
  delete[] filesInfo;
}

void FileTransferMainDialog::onRefreshLocalButtonClick()
{
  refreshLocalFileList();
}

void FileTransferMainDialog::onUploadButtonClick()
{
  unsigned int siCount = m_localFileListView.getSelectedItemsCount();

  if (siCount == 0) {
    MessageBox(m_ctrlThis.getWindow(),
               _T("No files selected."),
               _T("Upload Files"), MB_OK | MB_ICONWARNING);
    return ;
  }

  int *indexes = new int[siCount];
  FileInfo *filesInfo = new FileInfo[siCount];

  m_localFileListView.getSelectedItemsIndexes(indexes);
  for (unsigned int i = 0; i < siCount; i++) {
    FileInfo *fileInfo = reinterpret_cast<FileInfo *>(m_localFileListView.getItemData(indexes[i]));
    filesInfo[i] = *fileInfo;
  }

  if (MessageBox(m_ctrlThis.getWindow(),
                 _T("Do you wish to upload the selected files?"),
                 _T("Upload Files"),
                 MB_YESNO | MB_ICONQUESTION) != IDYES) {
    delete[] indexes;
    delete[] filesInfo;
    return ;
  }

  StringStorage localFolder;
  getPathToCurrentLocalFolder(&localFolder);

  StringStorage remoteFolder;
  getPathToCurrentRemoteFolder(&remoteFolder);


  m_fileExistDialog.resetDialogResultValue();

  m_ftCore->uploadOperation(filesInfo, siCount,
                            localFolder.getString(),
                            remoteFolder.getString());
  delete[] indexes;
  delete[] filesInfo;
}

void FileTransferMainDialog::onDownloadButtonClick()
{
  unsigned int siCount = m_remoteFileListView.getSelectedItemsCount();

  if (siCount == 0) {
    MessageBox(m_ctrlThis.getWindow(),
               _T("No files selected."),
               _T("Download Files"), MB_OK | MB_ICONWARNING);
    return ;
  }

  int *indexes = new int[siCount];
  FileInfo *filesInfo = new FileInfo[siCount];

  m_remoteFileListView.getSelectedItemsIndexes(indexes);
  for (unsigned int i = 0; i < siCount; i++) {
    FileInfo *fileInfo = reinterpret_cast<FileInfo *>(m_remoteFileListView.getItemData(indexes[i]));
    filesInfo[i] = *fileInfo;
  }

  if (MessageBox(m_ctrlThis.getWindow(),
                 _T("Do you wish to download the selected files?"),
                 _T("Download Files"),
                 MB_YESNO | MB_ICONQUESTION) != IDYES) {
    delete[] indexes;
    delete[] filesInfo;
    return ;
  }

  StringStorage remoteFolder;
  getPathToCurrentRemoteFolder(&remoteFolder);

  StringStorage localFolder;
  getPathToCurrentLocalFolder(&localFolder);

  m_fileExistDialog.resetDialogResultValue();

  m_ftCore->downloadOperation(filesInfo, siCount,
                              localFolder.getString(),
                              remoteFolder.getString());

  delete[] indexes;
  delete[] filesInfo;
}

void FileTransferMainDialog::moveUpLocalFolder()
{
  StringStorage pathToFile;
  getPathToParentLocalFolder(&pathToFile);
  tryListLocalFolder(pathToFile.getString());
}

void FileTransferMainDialog::moveUpRemoteFolder()
{
  StringStorage parent;
  getPathToParentRemoteFolder(&parent);
  tryListRemoteFolder(parent.getString());
}

void FileTransferMainDialog::onRemoteListViewDoubleClick()
{
  FileInfo *selFileInfo = m_remoteFileListView.getSelectedFileInfo();
  if (selFileInfo == 0)
    return;

  if (!selFileInfo->isDirectory())
    return;

  int si = m_remoteFileListView.getSelectedIndex();

  // Fake ".." folder - move one folder up
  if ((si == 0) && (selFileInfo != 0) && (_tcscmp(selFileInfo->getFileName(), _T(".."))) == 0) {
    moveUpRemoteFolder();
    return ;
  }
  if (si == -1) {
    return ;
  }
  StringStorage pathToFile;
  getPathToSelectedRemoteFile(&pathToFile);
  tryListRemoteFolder(pathToFile.getString());
}

void FileTransferMainDialog::onLocalListViewDoubleClick()
{
  // FIXME: removed duplicate code (see onRemoteListViewDoubleClick)
  FileInfo *selFileInfo = m_localFileListView.getSelectedFileInfo();

  if (selFileInfo == 0)
    return;

  if (!selFileInfo->isDirectory())
    return;

  int si = m_localFileListView.getSelectedIndex();

  // Fake ".." folder - move one folder up
  if ((si == 0) && (selFileInfo != 0) && (_tcscmp(selFileInfo->getFileName(), _T(".."))) == 0) {
    moveUpLocalFolder();
    return ;
  }
  if (si == -1) {
    return ;
  }

  StringStorage pathToFile;
  getPathToSelectedLocalFile(&pathToFile);
  tryListLocalFolder(pathToFile.getString());
}

void FileTransferMainDialog::onRemoteListViewKeyDown(UINT key)
{
  switch (key) {
  case VK_RETURN:
    onRemoteListViewDoubleClick();
    break;
  case VK_DELETE:
    onRemoveRemoteButtonClick();
    break;
  case VK_BACK:
    moveUpRemoteFolder();
    break;
  }
}

void FileTransferMainDialog::onLocalListViewKeyDown(UINT key)
{
  // FIXME: removed duplicate code (see onRemoteListViewKeyDown)
  switch (key) {
  case VK_RETURN:
    onLocalListViewDoubleClick();
    break;
  case VK_DELETE:
    onRemoveLocalButtonClick();
    break;
  case VK_BACK:
    moveUpLocalFolder();
    break;
  }
}

void FileTransferMainDialog::checkRemoteListViewSelection()
{
  if (m_ftCore->isNothingState()) {

    bool enabled = m_remoteFileListView.getSelectedItemsCount() > 0;

    m_renameRemoteButton.setEnabled(enabled && m_ftCore->getSupportedOps().isRenameSupported());
    m_removeRemoteButton.setEnabled(enabled && m_ftCore->getSupportedOps().isRemoveSupported());
  }
}

void FileTransferMainDialog::checkLocalListViewSelection()
{
  if (m_ftCore->isNothingState()) {
    bool enabled = m_localFileListView.getSelectedItemsCount() > 0;

    m_renameLocalButton.setEnabled(enabled);
    m_removeLocalButton.setEnabled(enabled);
  }
}

void FileTransferMainDialog::insertMessageIntoComboBox(const TCHAR *message)
{
  m_logComboBox.insertItem(0, message);
  m_logComboBox.setSelectedItem(0);
}

void FileTransferMainDialog::enableControls(bool enabled)
{
  m_mkDirRemoteButton.setEnabled(enabled && m_ftCore->getSupportedOps().isMkDirSupported());

  if (m_remoteFileListView.getSelectedItemsCount() > 0 && enabled) {
    m_renameRemoteButton.setEnabled(true && m_ftCore->getSupportedOps().isRenameSupported());
    m_removeRemoteButton.setEnabled(true && m_ftCore->getSupportedOps().isRemoveSupported());
  } else {
    m_renameRemoteButton.setEnabled(enabled && m_ftCore->getSupportedOps().isRenameSupported());
    m_removeRemoteButton.setEnabled(enabled && m_ftCore->getSupportedOps().isRemoveSupported());
  }

  m_refreshRemoteButton.setEnabled(enabled);

  if (enabled) {
    StringStorage curLocalPath;

    getPathToCurrentLocalFolder(&curLocalPath);

    if (!curLocalPath.isEmpty()) {
      m_mkDirLocalButton.setEnabled(true);
    }
  } else {
    m_mkDirLocalButton.setEnabled(enabled);
  }

  if (m_localFileListView.getSelectedItemsCount() > 0 && enabled) {
    m_renameLocalButton.setEnabled(true);
    m_removeLocalButton.setEnabled(true);
  } else {
    m_renameLocalButton.setEnabled(enabled);
    m_removeLocalButton.setEnabled(enabled);
  }

  m_refreshLocalButton.setEnabled(enabled);

  m_uploadButton.setEnabled(enabled && m_ftCore->getSupportedOps().isUploadSupported());
  m_downloadButton.setEnabled(enabled && m_ftCore->getSupportedOps().isDownloadSupported());

  m_localFileListView.setEnabled(enabled);
  m_remoteFileListView.setEnabled(enabled);

  m_cancelButton.setEnabled(!enabled);
}

void FileTransferMainDialog::initControls()
{
  HWND hwnd = m_ctrlThis.getWindow();

  m_renameRemoteButton.setWindow(GetDlgItem(hwnd, IDC_RENAME_REMOTE_BUTTON));
  m_mkDirRemoteButton.setWindow(GetDlgItem(hwnd, IDC_MKDIR_REMOTE_BUTTON));
  m_removeRemoteButton.setWindow(GetDlgItem(hwnd, IDC_REMOVE_REMOTE_BUTTON));
  m_refreshRemoteButton.setWindow(GetDlgItem(hwnd, IDC_REFRESH_REMOTE_BUTTON));

  m_renameLocalButton.setWindow(GetDlgItem(hwnd, IDC_RENAME_LOCAL_BUTTON));
  m_mkDirLocalButton.setWindow(GetDlgItem(hwnd, IDC_MKDIR_LOCAL_BUTTON));
  m_removeLocalButton.setWindow(GetDlgItem(hwnd, IDC_REMOVE_LOCAL_BUTTON));
  m_refreshLocalButton.setWindow(GetDlgItem(hwnd, IDC_REFRESH_LOCAL_BUTTON));

  m_uploadButton.setWindow(GetDlgItem(hwnd, IDC_UPLOAD_BUTTON));
  m_downloadButton.setWindow(GetDlgItem(hwnd, IDC_DOWNLOAD_BUTTON));

  m_cancelButton.setWindow(GetDlgItem(hwnd, IDC_CANCEL_BUTTON));

  m_copyProgressBar.setWindow(GetDlgItem(hwnd, IDC_TOTAL_PROGRESS));
  m_copyProgressBar.setRange(0, 1000);

  m_logComboBox.setWindow(GetDlgItem(hwnd, IDC_LOG_COMBO));

  m_localCurFolderTextBox.setWindow(GetDlgItem(hwnd, IDC_LOCAL_CURRENT_FOLDER_EDIT));
  m_remoteCurFolderTextBox.setWindow(GetDlgItem(hwnd, IDC_REMOTE_CURRENT_FOLDER_EDIT));

  m_localFileListView.setWindow(GetDlgItem(hwnd, IDC_LOCAL_FILE_LIST));
  m_remoteFileListView.setWindow(GetDlgItem(hwnd, IDC_REMOTE_FILE_LIST));

  m_fileExistDialog.setParent(&m_ctrlThis);
}

void FileTransferMainDialog::raise(Exception &ex)
{
  MessageBox(m_ctrlThis.getWindow(), ex.getMessage(),
             _T("Exception"), MB_OK | MB_ICONERROR);
  throw ex;
}

void FileTransferMainDialog::refreshLocalFileList()
{
  StringStorage pathToFile;
  getPathToCurrentLocalFolder(&pathToFile);
  tryListLocalFolder(pathToFile.getString());
}

void FileTransferMainDialog::tryListLocalFolder(const TCHAR *pathToFile)
{
  try {
    vector <FileInfo> *localFileList = m_ftCore->getListLocalFolder(pathToFile);


    // Add to list view
    m_localFileListView.clear();
    if (!localFileList->empty()) {
      FileInfo *fileInfo = &localFileList->front();
      m_localFileListView.addRange(&fileInfo,
                                   localFileList->size());
    }

    bool isRoot = (_tcscmp(pathToFile, _T("")) == 0);

    // Add ".." folder and if not root
    if (!isRoot) {
      m_localFileListView.addItem(0, m_fakeMoveUpFolder);
    }
    // Set label text
    m_localCurFolderTextBox.setText(pathToFile);
    // Enable or disable mkdir button depending on isRoot flag
    m_mkDirLocalButton.setEnabled(!isRoot);

  } catch (...) {
    StringStorage message;

    message.format(_T("Error: failed to get file list in local folder '%s'"),
                   pathToFile);

    insertMessageIntoComboBox(message.getString());
    return;
  }
}

void FileTransferMainDialog::refreshRemoteFileList()
{
  StringStorage currentFolder;
  m_remoteCurFolderTextBox.getText(&currentFolder);
  tryListRemoteFolder(currentFolder.getString());
}

void FileTransferMainDialog::tryListRemoteFolder(const TCHAR *pathToFile)
{
  m_lastSentFileListPath.setString(pathToFile);
  m_ftCore->remoteFileListOperation(pathToFile);
}

void FileTransferMainDialog::getPathToCurrentLocalFolder(StringStorage *out)
{
  m_localCurFolderTextBox.getText(out);
}

void FileTransferMainDialog::getPathToParentLocalFolder(StringStorage *out)
{
  getPathToCurrentLocalFolder(out);
  size_t ld = out->findLast(_T('\\'));
  if (ld != (size_t)-1) {
    out->getSubstring(out, 0, ld);  
  } else {
    out->setString(_T(""));
    return;
  }
  if (out->endsWith('\\') && (out->getLength() > 2)) {
    out->getSubstring(out, 0, out->getLength() - 2);
  }
}

void FileTransferMainDialog::getPathToSelectedLocalFile(StringStorage *out)
{
  StringStorage *pathToFile = out;
  getPathToCurrentLocalFolder(pathToFile);

  if (!pathToFile->isEmpty() && !pathToFile->endsWith(_T('\\'))) {
    pathToFile->appendString(_T("\\"));
  }

  const TCHAR *filename = m_localFileListView.getSelectedFileInfo()->getFileName();
  pathToFile->appendString(filename);
}

void FileTransferMainDialog::getPathToCurrentRemoteFolder(StringStorage *out)
{
  m_remoteCurFolderTextBox.getText(out);
}

void FileTransferMainDialog::getPathToParentRemoteFolder(StringStorage *out)
{
  getPathToCurrentRemoteFolder(out);
  size_t ld = out->findLast(_T('/'));
  if (ld != (size_t)-1) {
    out->getSubstring(out, 0, ld);  
  } else {
    out->setString(_T("/"));
    return ;
  }
  if (out->endsWith('/') && (out->getLength() > 2)) {
    out->getSubstring(out, 0, out->getLength() - 2);
  }
}

void FileTransferMainDialog::getPathToSelectedRemoteFile(StringStorage *out)
{
  StringStorage *pathToFile = out;
  getPathToCurrentRemoteFolder(pathToFile);

  if (!pathToFile->endsWith(_T('/'))) {
    pathToFile->appendString(_T("/"));
  }

  const TCHAR *filename = m_remoteFileListView.getSelectedFileInfo()->getFileName();
  pathToFile->appendString(filename);
}

void FileTransferMainDialog::setNothingState()
{
  m_lastReceivedFileListPath = m_lastSentFileListPath;
  m_remoteCurFolderTextBox.setText(m_lastReceivedFileListPath.getString());

  m_remoteFileListView.clear();
  vector<FileInfo> *fileRemoteList = m_ftCore->getListRemoteFolder();
  if (!fileRemoteList->empty()) {
    FileInfo *filesInfo = &fileRemoteList->front();
    m_remoteFileListView.addRange(&filesInfo, fileRemoteList->size());
  }

  bool isRoot = m_lastSentFileListPath.isEqualTo(_T("/"));

  // Add fake ".." folder if not root
  if (!isRoot) {
    m_remoteFileListView.addItem(0, m_fakeMoveUpFolder);
  }
}

void FileTransferMainDialog::onFtOpError(const TCHAR *message)
{
  insertMessageIntoComboBox(message);
}

void FileTransferMainDialog::onFtOpInfo(const TCHAR *message)
{
  insertMessageIntoComboBox(message);
}

void FileTransferMainDialog::onFtOpStarted()
{
  enableControls(false);
}

void FileTransferMainDialog::onFtOpFinished(int state, int result)
{
  PostMessage(m_ctrlThis.getWindow(), WM_OPERATION_FINISHED, state, result);
}

void FileTransferMainDialog::onRefreshLocalFileList()
{
  refreshLocalFileList();
}
void FileTransferMainDialog::onRefreshRemoteFileList()
{
  refreshRemoteFileList();
}
