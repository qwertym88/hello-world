<https://www.zhangxinxu.com/wordpress/2018/05/live2d-web-webgl-js/>

<https://blog.csdn.net/suifcd/article/details/79175189?utm_source=blogxgwz2>

！！！！

<https://github.com/EYHN/hexo-helper-live2d/blob/master/README.zh-CN.md>

<https://zh.moegirl.org/Live2D>

### 重点
它这个测试程序有点坑，必须要一个服务器（本地iis）才能加载（其实是chrome干得好事）
然后这个网站其实直接改那个wwwroot就行了，webconfig自动生成的也有点坑
那个moc文件读取不了，老是返回404
要加一个mine类型，<del>然后还要改那个webconfig</del>,事实上不用，具体大概就那样吧。

终于把所有资料打包到云盘了

## 欢天喜地最后一个月
成功了一半吧。发现官方的api实在太恼火了，改的地方有点小多。以sampleapp1为例。
感谢<https://blog.csdn.net/Kakinuma/article/details/50697762>
### sampleApp1.js
这个就是主js了。里面定义以下功能
* 换角色（如不想换得删代码）
* 测试数据log和error（然鹅只要把LAppDifine中DEBUG_LOG改为false即可）

其他的也可以改，反正就是删代码的事。
其中还有一些没有理解的，加油吧。

### LAppDefine.js
其实它改名.json才对，因为它仅仅充当一个配置的作用，相当于一个config。我们的模型数据就在其中。主要就是模型数据，动作，交互设置等，还有一些程序api。

### LAppModel.js
说实话我没看过这个。相当于是上一个的包装，大概就是这样吧。

### LAppLive2DManager.js
人如其名。在其中可以更改备选的模型。

### PlatformManager.js live2d.min.js Live2DFramework.js
官方api，改动几率不大（应该吧）

细节还不对，人物只显示了部分，动作没有绑定，再接再厉吧。

------
目前看有两个api，一个是<https://github.com/galnetwen/Live2D>,一个用webgl渲染，结合pixi的东西。看着有点复杂。
还有一个<https://github.com/fghrsh/live2d_api>，php的，目前实在看不懂。
