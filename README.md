# **Road vehicles — Diagnostic**

## **介绍**
- **communication over Controller Area**
- **Network (DoCAN) —Part 2: Transport protocol and network layer services**

## **测试环境搭建**
### 测试平台
- _Visual Studio 2010_
- _Linux environment_

### 环境搭建
#### Windows Platform
- **为VS2010添加pthread库**
	- 拷贝文件夹docs/pthreads-w32-2-9-1-release.zip/Pre-built.2到VS2010安装目录（例如D:）：D:\Microsoft Visual Studio 10.0\VC\lib下；
	- Project->Properties->Configuration Prooerties->VC++ Directories，设置Include Directories，则在包含目录那一栏添加：D:\Microsoft Visual Studio 10.0\VC\lib\Pre-built.2\include;在库目录那一栏添加：D:\Microsoft Visual Studio 10.0\VC\lib\Pre-built.2\lib\x86
	- 找到Project->Properties->Configuration Prooerties->Linker->Input找到Additional Dependenceies添加库文件名pthreadVC2.lib;pthreadVCE2.lib;pthreadVSE2.lib;

- **设置工程环境**
	- 打开vs2010，Project->Properties->Configuration Prooerties->VC++ Directories，设置Include Directories，则在包含isotp工程中所有包含head_name.h的目录;
	- 打开D:\Microsoft Visual Studio 10.0\VC\lib\Pre-built.2\lib\dll\x86\，将里面pthreadVC2.dll文件VS2010工程目录；

#### Linux Platform
- **编译**
	- 确定运行平台并指定编译器，将编译器写入到make.sh文件中，执行./make.sh

### 参考标准
- 《371571297-ISO-15765-2-Road-vehicles-Diagnostics-on-CAN-pdf.pdf》
- 《车载诊断标准ISO+15765-2中文.docx》

## 结束