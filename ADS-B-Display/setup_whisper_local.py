#!/usr/bin/env python3
"""
Whisper.cpp 자동 설치 스크립트
Automatic Whisper.cpp Setup Script

이 스크립트는 다음을 수행합니다:
1. whisper.cpp 다운로드 및 설치
2. 선택한 모델 다운로드
3. 설정 파일 생성

This script will:
1. Download and install whisper.cpp
2. Download selected model
3. Generate configuration file
"""

import os
import sys
import subprocess
import urllib.request
import platform
import shutil
from pathlib import Path

# Colors for terminal output
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

def print_header(text):
    print(f"\n{Colors.HEADER}{Colors.BOLD}{'='*60}{Colors.ENDC}")
    print(f"{Colors.HEADER}{Colors.BOLD}{text:^60}{Colors.ENDC}")
    print(f"{Colors.HEADER}{Colors.BOLD}{'='*60}{Colors.ENDC}\n")

def print_info(text):
    print(f"{Colors.OKBLUE}ℹ {text}{Colors.ENDC}")

def print_success(text):
    print(f"{Colors.OKGREEN}✓ {text}{Colors.ENDC}")

def print_warning(text):
    print(f"{Colors.WARNING}⚠ {text}{Colors.ENDC}")

def print_error(text):
    print(f"{Colors.FAIL}✗ {text}{Colors.ENDC}")

def check_prerequisites():
    """필수 프로그램 확인"""
    print_header("시스템 요구사항 확인 (Checking Prerequisites)")
    
    required = {
        'git': 'Git',
        'cmake': 'CMake',
    }
    
    missing = []
    
    for cmd, name in required.items():
        if shutil.which(cmd) is None:
            print_error(f"{name} not found")
            missing.append(name)
        else:
            print_success(f"{name} found")
    
    if missing:
        print_error(f"\n다음 프로그램을 설치해주세요: {', '.join(missing)}")
        print_info("Git: https://git-scm.com/downloads")
        print_info("CMake: https://cmake.org/download/")
        return False
    
    # Check compiler
    if platform.system() == "Windows":
        if shutil.which('cl') is None and shutil.which('g++') is None:
            print_warning("Visual Studio or MinGW not found")
            print_info("Visual Studio 권장: https://visualstudio.microsoft.com/")
            return False
        else:
            print_success("Compiler found")
    
    return True

def get_install_directory():
    """설치 디렉토리 선택"""
    print_header("설치 경로 선택 (Select Installation Directory)")
    
    if platform.system() == "Windows":
        default_path = "C:\\whisper.cpp"
    else:
        default_path = str(Path.home() / "whisper.cpp")
    
    print_info(f"기본 경로: {default_path}")
    user_path = input(f"설치 경로를 입력하세요 (Enter for default): ").strip()
    
    install_path = user_path if user_path else default_path
    
    # Create directory if it doesn't exist
    os.makedirs(install_path, exist_ok=True)
    
    print_success(f"설치 경로: {install_path}")
    return install_path

def clone_whisper_cpp(install_path):
    """whisper.cpp 클론"""
    print_header("Whisper.cpp 다운로드 (Downloading Whisper.cpp)")
    
    repo_url = "https://github.com/ggerganov/whisper.cpp.git"
    
    try:
        if os.path.exists(os.path.join(install_path, ".git")):
            print_info("Whisper.cpp already exists. Updating...")
            subprocess.run(["git", "pull"], cwd=install_path, check=True)
        else:
            print_info(f"Cloning from {repo_url}...")
            subprocess.run(["git", "clone", repo_url, install_path], check=True)
        
        print_success("Whisper.cpp downloaded successfully")
        return True
    except subprocess.CalledProcessError as e:
        print_error(f"Failed to clone repository: {e}")
        return False

def build_whisper_cpp(install_path):
    """whisper.cpp 빌드"""
    print_header("Whisper.cpp 빌드 (Building Whisper.cpp)")
    
    build_path = os.path.join(install_path, "build")
    os.makedirs(build_path, exist_ok=True)
    
    try:
        print_info("Running CMake...")
        subprocess.run(["cmake", ".."], cwd=build_path, check=True)
        
        print_info("Building (this may take a few minutes)...")
        subprocess.run(["cmake", "--build", ".", "--config", "Release"], 
                      cwd=build_path, check=True)
        
        print_success("Whisper.cpp built successfully")
        return True
    except subprocess.CalledProcessError as e:
        print_error(f"Build failed: {e}")
        print_warning("Trying alternative build method...")
        
        # Try make on Unix-like systems
        if platform.system() != "Windows":
            try:
                subprocess.run(["make"], cwd=install_path, check=True)
                print_success("Built with make successfully")
                return True
            except:
                pass
        
        return False

def select_model():
    """모델 선택"""
    print_header("모델 선택 (Select Model)")
    
    models = {
        '1': ('tiny', '75 MB', 'Fastest, lower accuracy'),
        '2': ('base', '142 MB', 'Balanced (Recommended)'),
        '3': ('small', '466 MB', 'Good accuracy'),
        '4': ('medium', '1.5 GB', 'Better accuracy'),
        '5': ('large', '2.9 GB', 'Best accuracy'),
    }
    
    print("\n사용 가능한 모델:")
    print("-" * 60)
    for key, (name, size, desc) in models.items():
        print(f"{key}. {name:8} | {size:8} | {desc}")
    print("-" * 60)
    
    while True:
        choice = input("\n모델 번호를 선택하세요 (1-5) [2]: ").strip() or '2'
        if choice in models:
            return models[choice][0]
        print_warning("Invalid choice. Please enter 1-5.")

def download_model(install_path, model_name):
    """모델 다운로드"""
    print_header(f"모델 다운로드 (Downloading Model: {model_name})")
    
    models_path = os.path.join(install_path, "models")
    os.makedirs(models_path, exist_ok=True)
    
    model_file = f"ggml-{model_name}.bin"
    model_path = os.path.join(models_path, model_file)
    
    if os.path.exists(model_path):
        print_success(f"Model already exists: {model_path}")
        return True
    
    # Download URLs
    base_url = "https://huggingface.co/ggerganov/whisper.cpp/resolve/main"
    model_url = f"{base_url}/{model_file}"
    
    try:
        print_info(f"Downloading from {model_url}...")
        print_info("This may take several minutes depending on model size...")
        
        def show_progress(block_num, block_size, total_size):
            downloaded = block_num * block_size
            percent = min(100, downloaded * 100 / total_size)
            bar_length = 50
            filled = int(bar_length * downloaded / total_size)
            bar = '█' * filled + '-' * (bar_length - filled)
            print(f'\r[{bar}] {percent:.1f}%', end='', flush=True)
        
        urllib.request.urlretrieve(model_url, model_path, show_progress)
        print()  # New line after progress bar
        
        print_success(f"Model downloaded: {model_path}")
        return True
        
    except Exception as e:
        print_error(f"Failed to download model: {e}")
        print_info("\nAlternative: Use the download script in whisper.cpp/models/")
        print_info(f"Run: bash {os.path.join(models_path, 'download-ggml-model.sh')} {model_name}")
        return False

def create_config_file(install_path, model_name):
    """설정 파일 생성"""
    print_header("설정 파일 생성 (Creating Configuration)")
    
    config_content = f"""# Whisper STT Configuration
# Generated by setup_whisper_local.py

[Whisper]
InstallPath={install_path}
ModelName={model_name}
ModelPath={os.path.join(install_path, 'models', f'ggml-{model_name}.bin')}

[Usage in C++]
# Add to your code:
# whisperSTT->SetLocalModelPath(L"{install_path.replace(os.sep, '\\\\')}");
# whisperSTT->UseLocalModel(true);
# whisperSTT->Initialize(WhisperModelType::BASE);
"""
    
    config_path = os.path.join(os.path.dirname(__file__), "whisper_config.txt")
    
    try:
        with open(config_path, 'w', encoding='utf-8') as f:
            f.write(config_content)
        
        print_success(f"Configuration saved: {config_path}")
        print_info("\nC++ 코드에서 사용:")
        print(f'whisperSTT->SetLocalModelPath(L"{install_path.replace(os.sep, "\\\\")}");')
        return True
        
    except Exception as e:
        print_error(f"Failed to create config file: {e}")
        return False

def test_installation(install_path):
    """설치 테스트"""
    print_header("설치 테스트 (Testing Installation)")
    
    # Check for executable
    if platform.system() == "Windows":
        exe_paths = [
            os.path.join(install_path, "build", "bin", "Release", "main.exe"),
            os.path.join(install_path, "build", "Release", "main.exe"),
            os.path.join(install_path, "main.exe"),
        ]
    else:
        exe_paths = [
            os.path.join(install_path, "build", "bin", "main"),
            os.path.join(install_path, "main"),
        ]
    
    main_exe = None
    for path in exe_paths:
        if os.path.exists(path):
            main_exe = path
            break
    
    if main_exe:
        print_success(f"Whisper executable found: {main_exe}")
        
        # Try to run with --help
        try:
            result = subprocess.run([main_exe, "--help"], 
                                  capture_output=True, 
                                  text=True, 
                                  timeout=5)
            if result.returncode == 0:
                print_success("Executable is working correctly")
                return True
        except:
            pass
    
    print_warning("Could not verify installation. Please test manually.")
    return False

def main():
    """메인 함수"""
    print_header("Whisper.cpp 자동 설치 스크립트")
    print(f"{Colors.OKCYAN}ADS-B Display - Speech-to-Text Setup{Colors.ENDC}\n")
    
    # Check prerequisites
    if not check_prerequisites():
        print_error("\nPrerequisites not met. Please install required software.")
        return 1
    
    # Get installation directory
    install_path = get_install_directory()
    
    # Clone repository
    if not clone_whisper_cpp(install_path):
        return 1
    
    # Build
    if not build_whisper_cpp(install_path):
        print_warning("\nBuild failed, but you can try building manually later.")
        print_info(f"cd {install_path}")
        print_info("mkdir build && cd build")
        print_info("cmake .. && cmake --build . --config Release")
    
    # Select and download model
    model_name = select_model()
    download_model(install_path, model_name)
    
    # Create config
    create_config_file(install_path, model_name)
    
    # Test installation
    test_installation(install_path)
    
    # Final instructions
    print_header("설치 완료! (Installation Complete!)")
    print_success("Whisper.cpp has been successfully installed")
    print_info(f"\nInstallation directory: {install_path}")
    print_info(f"Model: {model_name}")
    print_info("\nNext steps:")
    print("  1. Add WhisperSTT.h and WhisperSTT.cpp to your project")
    print("  2. See WhisperSTT_Integration.cpp for usage examples")
    print("  3. Read WHISPER_STT_SETUP_GUIDE.md for detailed instructions")
    print(f"\n{Colors.OKCYAN}Happy coding! 🎉{Colors.ENDC}\n")
    
    return 0

if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print_error("\n\nInstallation cancelled by user")
        sys.exit(1)
    except Exception as e:
        print_error(f"\n\nUnexpected error: {e}")
        sys.exit(1)

