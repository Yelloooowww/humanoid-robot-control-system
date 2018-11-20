# -*- coding: utf-8 -*-
import os
import PyInstaller.__main__
from shutil import copyfile

opts = [
    # NOTE '-F' maybe not stable in some environment, need more test.
    '-F',

    # NOTE if you want exe not to print info to I/O console, use '-w' .
    # '-w',

    # NOTE if ypu want to extra debug info, use the '--debug'.
    # 沒啥用，但可以試試看
    # '--debug',


    # NOTE you need to change the libs path.
    # If it has been include in yout system PATH, you don't have to add it.
    # Need Windows Kits 10 to support win10.
    '--paths=C:\\Users\\user\\AppData\\Local\\Programs\\Python\\Python36\\Lib\\site-packages\\PyQt5\\Qt\\bin',
    '--paths=C:\\Users\\user\\AppData\\Local\\Programs\\Python\\Python36\\Lib\\site-packages\\PyQt5\\Qt\\plugins',
    '--paths=C:\\Users\\user\\AppData\\Local\\Programs\\Python\\Python36\\Lib\\site-packages\\PyInstaller\\bootloader\\Windows-32bit',
    # '--paths=C:\\Users\\Lite\\AppData\\Roaming\\pyinstaller\\bincache00_py36_32bit',
    '--paths=C:\\Users\\user\\AppData\\Roaming\\pyinstaller\\bincache00_py36_64bit',
    # '--paths=C:\\Program Files (x86)\\Windows Kits\\10\\Redist\\ucrt\\DLLs\\x86',
    # '--paths=C:\\Program Files (x86)\\Windows Kits\\10\\Redist\\ucrt\\DLLs\\x64',
    '--paths=C:\\Program Files (x86)\\Windows Kits\\10\\Redist\\10.0.17763.0\\ucrt\\DLLs\\x64',
    '--paths=C:\\Program Files (x86)\\Windows Kits\\10\\Redist\\10.0.17763.0\\ucrt\\DLLs\\x86',

    # NOTE Need DLLs in package numpy and scipy.
    # '--paths=D:\\Programs\\Python\\Python36-32\\Lib\\site-packages\\scipy\\extra-dll',
    # '--paths=D:\\Programs\\Python\\Python36-32\\Lib\\site-packages\\numpy\\.libs',
    # # NOTE Need extra moudle in package scipy.
    # '--hidden-import=scipy._lib.messagestream',

    # add icon if you want
    # '--icon', 'rxx.ico',

    # NOTE not use UPX even if it is available (works differently between Windows and *nix)
    '--noupx',

    # NOTE Clean PyInstaller cache and remove temporary files before building
    # 一定要加
    '--clean',

    # NOTE your application entry file
    'pyqtapp/__main__.py'
]

if __name__ == '__main__':
    PyInstaller.__main__.run(opts)
    copyfile(icon.ico, icon.ico)
    copyfile(_background.jpg, _background.jpg)
