DBC 文件解析与 CAN 信号仿真工具
基于Qt6开发的图形化工具，用于解析标准DBC (CAN Database) 文件，支持信号值的输入、物理值与原始值的转换，并按CAN帧格式打包后通过 WebSocket发送至服务端。支持JSON和Protobuf两种序列化格式。

一、功能特性：
1、DBC文件解析：
加载并解析标准DBC文件，提取Message（ID、名称、DLC）以及Signal（起始位、长度、字节序、因子、偏移、最小值、最大值、单位）。
2、树形/表格展示：
左侧树形控件显示所有Message，右侧表格显示选中Message的Signal属性。
3、信号输入与打包：
为每个Signal提供物理值输入控件，自动换算为原始值，并按Intel/ Motorola 字节序打包成CAN帧数据字节。
4、WebSocket发送：
支持连接到指定WebSocket服务端，将打包后的帧数据以JSON或Protobuf格式发送。
5、范围校验：
对输入的物理值进行范围校验（根据DBC中的 min/max），超限时给出提示。
6、多标签页管理：
支持同时打开多个Message，每个标签页独立保存信号输入值。
7、连接状态显示：
实时显示WebSocket连接状态（已连接/断开/错误）。
8、发送日志面板：
记录每次发送的时间、Message ID和名称。

二、环境要求：
1、操作系统：Windows 10 / 11（已测试）；
2、编译器：MSVC 2022（64-bit）；
3、Qt 版本：6.10.1或更高（需包含以下模块：Core, Widgets, Network, 
WebSockets, Protobuf）；
4、开发工具：Qt Creator；
5、说明：本项目使用 Qt 6 内置的CMake和Protobuf支持，无需用户单独安装 CMake 或 Protocol Buffers 工具；
6、注意实现：
（1）项目路径不能包含中文字符或空格，否则可能导致qtprotobufgen或protoc解析失败。
（2）如果遇到Protobuf相关错误，请检查系统PATH中是否有外部 Protobuf 安装干扰，并确保使用 Qt 自带的qtprotobufgen.exe。

三、依赖安装：
本项目仅依赖 Qt 6.10.1 及以上版本（需包含 Core, Widgets, Network, WebSockets, Protobuf 模块）。请按以下步骤安装：
1.1 从Qt官网下载在线安装器（Qt Online Installer）；
1.2 安装时选择Qt 6.10.1或更高版本，并勾选以下组件：
（1）MSVC 2022 64-bit；
（2）Qt WebSockets；
（3）Qt Protobuf；
1.3  安装完成后，记下Qt的安装路径（例如 C:\Qt\6.10.1\msvc2022_64）。

四、构建与运行：
本项目包含两个独立的 Qt 项目：客户端（DBC解析工具）和服务端（WebSocket打印服务）。
1、使用Qt Creator：
1.1 客户端：
（1）打开客户端项目：
启动Qt Creator，选择文件→打开文件或项目，找到项目根目录下的 CMakeLists.txt 并打开。
（2）配置构建套件：
Qt Creator会自动检测到可用的构建套件（Kit）。请确保选择的编译器为 MSVC 2022 64-bit（与安装 Qt 时选择的编译器一致）。
（3）构建并运行客户端：
点击左下角的绿色三角形（运行）按钮，或按Ctrl+R。Qt Creator会自动完成CMake配置、编译和运行。
1.2 服务端：
（1）打开服务端项目：
在另一个 Qt Creator 窗口中，打开服务端项目目录下的“CMakeLists.txt”。
（2）构建并运行服务端：
点击运行按钮启动服务端，默认监听“ws://localhost:8080”。
2、运行说明：
2.1 服务端：控制台输出“WebSocket server started at ws://localhost:8080 (supports JSON and Protobuf)”表示启动成功。
2.2 客户端：
（1）启动前可在Constants文件修改地址“DEFAULT_WEBSOCKET_URL”（默认已填），点击“连接”按钮。
（2）在地址栏输入 ws://localhost:8080（默认已填），点击 “连接” 按钮。
（3）加载一个 DBC 文件（参见下方“测试用 DBC 文件”），选择某条消息，输入信号物理值，点击“发送到服务端”。
（4）服务端控制台会打印接收到的数据（ JSON或Protobuf格式）。

五、测试用DBC文件：
1、在线示例集合：
（1）网址：”https://github.com/rundekugel/DBC-files“，收集了多个车型的示例 DBC 文件，可用于功能验证。
2、本地测试文件：
（1）invalid_1.dbc、invalid_2.dbc：包含语法错误，用于测试解析器的容错处理。
（2）correct_intel_1.dbc、correct_intel_2.dbc：格式正确，信号采用 Intel（小端）字节序。
（3）correct_motorola.dbc：格式正确，信号采用 Motorola（大端）字节序，可用于验证两种打包算法的正确性。

六、WebSocket 默认地址与端口
（1）客户端默认连接地址：ws://localhost:8080（可在Constants文件中修改地址“DEFAULT_WEBSOCKET_URL”（）。
（2）服务端默认监听地址：ws://localhost:8080（如需修改，请更改服务端源码中的端口号）。

七、项目结构：
1、客户端：
├── CMakeLists.txt               	 # 客户端主构建文件
├── main.cpp                     # 程序入口
├── dbctool.h / .cpp / .ui       		# 主窗口类
├── dbcparser.h / .cpp            	# DBC 文件解析器
├── canframebuilder.h / .cpp     	# CAN 帧打包（Intel / Motorola）
├── canwebsocketclient.h / .cpp  	# WebSocket 客户端封装
├── jsonserializer.h / .cpp      		# JSON 序列化
├── signalinputpanel.h / .cpp    	# 信号输入面板（动态生成）
├── tabmanager.h / .cpp          	# 多标签页管理
├── rangevalidator.h / .cpp      	# 范围校验
├── logger.h / .cpp              	# 日志记录
├── utils.h / .cpp               	# 工具函数
├── constants.h                  	# 常量定义
├── canframe.proto               	# Protobuf 协议定义
2、服务端：
├── CMakeLists.txt					# WebSocket 服务端
├── main.cpp
└── canframe.proto               	# Protobuf 协议定义

八、已知限制：
1、不支持DBC文件中的多路复用（Multiplexing / MUX）信号，解析时会跳过此类信号。
2、目前仅对物理值进行范围校验，不对原始值进行溢出检查（因位宽已在打包时自动截断）。
3、仅支持 Intel (小端) 和 Motorola (大端) 两种字节序，未处理 @0+等变体（视为Intel）。

九、致谢：
1、DBC测试文件来自 rundekugel/DBC-files；
2、Qt官方文档与 Qt Protobuf模块示例；
