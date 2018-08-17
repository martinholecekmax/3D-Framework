# 3D-Framework
Basic 3D Framework Win32 application written in C++ and using only GDI calls

![alt text](https://github.com/martinholecekmax/3D-Framework/blob/master/screenshot.gif "Application Screenshot")

## Installation
This framework has been written by using Visual Studio IDE. There are two features (Workloads) which have to be installed; otherwise, the project will not compile. The steps below show how to install them.
- On the Visual Studio menu bar, choose **Tools** > **Get Tools and Features** > **Workloads**
- Select the **Desktop development with C++** and at the right side in installation details select a checkbox **Windows SDK latest version**.
- Also, select the **Game development with C++**, and click on **Modify** (bottom right) to install these features

Lastly, the project must be configured to use the correct version of Windows SDK.
- In **Solution Explorer**, right click on **Rasteriser** and chose **Properties**
- Then in **Configuration Properties** > **General**, select **Windows SDK Version** to be the latest one and click on **OK** to confirm changes
