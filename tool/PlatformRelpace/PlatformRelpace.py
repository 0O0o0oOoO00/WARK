import subprocess
import os
import shutil
from pathlib import Path
from dataclasses import dataclass

# The RUN_IN_VISUAL_STUDIO and CONFIGURATION are defined
# in the property setting of this project in visual studio
# if run in visual studio

ROOT_DIR = Path(os.path.dirname(__file__))
if os.environ.get("RUN_IN_VISUAL_STUDIO") is not None:
    SRC_DIR = ROOT_DIR / "../../src"
    TOOL_DIR = ROOT_DIR / f"../../bin/{os.environ['CONFIGURATION']}/tool"
else:
    SRC_DIR = ROOT_DIR / "../../../src"
    TOOL_DIR = ROOT_DIR

@dataclass
class PeFile:
    pe_file: str
    pdb_file: str
    offset_file: str

class Logger:
    @staticmethod
    def log(message: str) -> None:
        print(message)
        
    @staticmethod    
    def success(message: str) -> None:
        Logger.log("[+] " + message)
        
    @staticmethod    
    def fail(message: str) -> None:
        Logger.log("[-] " + message)
        exit(-1)
        
    @staticmethod    
    def warning(message: str) -> None:
        Logger.log("[!] " + message)
        
    @staticmethod    
    def info(message: str) -> None:
        Logger.log("[*] " + message)
        

class ReplaceFile:
    def __init__(self, 
                 source_dir: str,
                 source_file_name: str,  
                 target_dir: str,
                 new_name: str | None = None
                 ) -> None:
        self.source_file_name = source_file_name
        self.source_dir = Path(source_dir)
        self.target_dir = Path(target_dir)
        self.new_name = new_name
        
        self.source_file = self.source_dir / self.source_file_name
        if new_name is None:
            self.target_file = self.target_dir / self.source_file_name
        else:
            self.target_file = self.target_dir / self.new_name
    def replace(self) -> None:
        shutil.move(self.source_file, self.target_file)

PE_FILE: list[PeFile] = [
    PeFile(str(ROOT_DIR / "ntoskrnl.exe"), str(ROOT_DIR / "ntkrnlmp.pdb"), str(ROOT_DIR / "ntkrnlmp.h")),
    ]

FILE_TO_CHECK = [
    str(TOOL_DIR / "DownloadPdb.exe"),
    str(TOOL_DIR / "ExtractOffset.exe"),
    str(ROOT_DIR / "ntoskrnl.exe"),
    ]

DIR_TO_CHECK = [
    str(TOOL_DIR),
    ]

FILE_TO_REPLACE: list[ReplaceFile] = [
    ReplaceFile(ROOT_DIR, "ntkrnlmp.h", SRC_DIR / "R0S", "R0S_NtoskrnlOffset.h"),
    ]

def download_pdb() -> None:
    for file in PE_FILE:
        Logger.info(f"Download {file.pdb_file}")
        subprocess.run([str(TOOL_DIR / "DownloadPdb.exe"), file.pe_file])
        if not os.path.exists(file.pdb_file):
            Logger.fail(f"Pdb of pe file download failed, pe file: {file.pe_file}")
    
def extract_offset() -> None:
    for file in PE_FILE:
        Logger.info(f"Extract {file.pdb_file}")
        subprocess.run([str(TOOL_DIR / "ExtractOffset.exe"), file.pdb_file])
        if not os.path.exists(file.offset_file):
            Logger.fail(f"Extract symbols offset failed, pdb file: {file.pdb_file}")

def run_command() -> None:
    download_pdb()
    extract_offset()
    
def replace_file() -> None:
    for file in FILE_TO_REPLACE:
        file.replace()
 
def parse_command_line() -> None:
    ...

def check_file_and_dir() -> None:
    for dir_ in DIR_TO_CHECK:
        if not os.path.exists(dir_):
            Logger.fail(f"Required dir doesn't exist, dir: {dir_}")
    for file in FILE_TO_CHECK:
        if not os.path.exists(file):
            Logger.fail(f"Required file doesn't exist, file: {file}")

def main():
    if os.environ.get("RUN_IN_VISUAL_STUDIO") is not None:
        Logger.info("Start by visual studio")
    else:
        Logger.info("Run in command line")

    parse_command_line()
    check_file_and_dir()
    run_command()
    replace_file()

if __name__ == "__main__":
    main()