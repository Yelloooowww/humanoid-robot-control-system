import  os
if __name__ == '__main__':
    from PyInstaller.__main__ import run
    opts=['__init__.py','__main__.py','ui_mainwindow_before.py','-w','-F','--icon=D:\PPPython\QT_project\pyqtapp\icon.ico']
    run(opts)
