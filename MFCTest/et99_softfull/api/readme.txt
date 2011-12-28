0、FT_ET99_API.h : ET99 API头文件定义
1、FT_ET99_API.lib ：ET99 API静态库
2、FT_ET99_API.dll ：ET99 API动态库
3、ET99_MOD.dll ：ET99全功能Active控件
4、ET99_FULL.dll ：ET99网页安全Active控件
5、JET99AI20.dll ：ET99 jni接口的java动态库


注：1、在运行ET99全功能Active控件和ET99网页安全Active控件之前，
    请把FT_ET99_API.dll、ET99_MOD.dll和ET99_FULL.dll拷贝到
    系统目录下，然后注册全功能Active控件和网页Active控件，
    命令行命令如下：    regsvr32 ET99_MOD.dll和
                        regsvr32 ET99_FULL.dll，
    注销控件命令如下：  regsvr32 ET99_MOD.dll -u 和
                        regsvr32 ET99_FULL.dll -u


    2、动态库JET99AI20.dll可以用来开发java应用程序，如sample目录下的jni示例，
    该动态库的接口定义在jni示例的类IET99.java中，可仿照示例程序进行开发java应用程序。
    java应用程序运行时请把该动态库拷贝到系统目录下。

    
    3、ET99_FULL.dll、ET99_MOD.dll、JET99AI20.dll都是建立在FT_ET99_API.dll基础上的，
    因此在使用前3个库时需要将FT_ET99_API.dll拷贝到相同目录下。