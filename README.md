# ShaderBrowser

## 2018.10.28 <br>
封装渲染引擎架构，很多还有待完成。<br>

## 2018.11.4~2018.11.9 <br>
封装了Texture2D，添加了引用计数机制 和 ESC框架，下面要完善ESC架构中的system和component。<br>

## 2018.11.20~2018.12.15 <br>
增加了对macOS的支持，加入了Assimp库，用于后续模型文件的读取<br>

## 2018.12.16 <br>
封装了uniform变量的结构体，修改了material的结构，在其中加入了pass，一个meterial能包含多个pass。不过目前只c支持执行第一个，后面在重写RenderSystem的时候会
考虑这块的逻辑。<br>

## 2018.12.23 <br>
封装了顶点属性的结构，优化了RenderSystem的逻辑，现在改为使用索引的方式来绘制。并且可以对纹理进行采样，但对多级渐远纹理的处理还没有实现。概念上理清了VAO
和渲染组件的对应关系(对于VAO来说，只需要顶点数组属性结构相同，都可以使用相同的vao)，后面对于拥有相同的顶点属性结构的渲染对象来说，都可以使用同一个vao，
为往后的合批处理提供了方便。 <br>

## 2018.12.30~2018.12.31 <br>
封装了Transform组件，但目前并不支持同一帧里去获得Transform的变换属性(因为这里都是在visit遍历的时候才计算)，后面还要继续优化。不过对于当中用到的Rotate方
法还有些疑问，虽然参照unity测试比较结果实现了，但对于model矩阵中的旋转矩阵的计算还是不太懂：Space.Self按照y-x-z的顺序旋转 和 Space.World按照z-x-y的顺序
旋转，得到的四元数为何都是 模型->惯性空间 的？？？？（个人认为是模型->惯性空间的依据，主要是因为使用了自己写的四元数转换欧拉角的函数，用到的是模型->惯性
空间的实现） <br>

## 2018.12.30~2018.12.31 <br>
完善Transform组件，实现了在逻辑阶段中获取设置组件属性的功能，与unity的结果完全相同。对于之前heading-pitch-bank旋转的问题，应该是glm是列主序的矩阵，需要
左乘，所以实际使用时顺序是z-y-x，所以得到的是一个 物体->惯性 的矩阵，后面将开始camera的封装，以及对模型的使用封装等。<br>

## 2019.1.11~2019.1.13 <br>
封装了MeshFilter组件，整理优化了纹理的uniform，理顺了纹理单元的逻辑(需要用glUniform1i来对纹理单元进行绑定，并且只需要绑定一次即可)。在这期间，发现之前使
用unordered_map时，使用了const char*来作为它的key值，这样做会造成问题(需要自己去实现unordered_map的hash方法和比较方法)，所以现在已经全部替换为std::string
来作为key值。对于MeshFilter组件，后面会继续完善，实现从模型文件中获取顶点数据和纹理信息。<br>

## 2019.1.18~2019.1.20 <br>
将MeshFilter拆分为多个Mesh。Mesh是封装顶点数据、纹理、VAO的最基础的单元，并对应一个Material的name。这些Material会在AddMesh或者给entity添加MeshFilter组件
的时候自动生成(Render组件和Material都只是容器，render的目的是根据mesh查找到对应的material，material则根据当前渲染队列找到对应的pass)。并添加了组件之间的
事件机制使不同组件之间可以相互通信，例如MeshFilter和Render组件之间。	<br>

## 2019.1.28 <br>
使用模板来创建单例和cache，加深了对模板的理解以及对c++编译链接的理解。通过模板，代码也变得更加精简。另外添加了文件管理系统，还没弄完，有待完善。	<br>

## 2019.1.29 <br>
完善FileUtils，添加异步加载资源逻辑，因为是模板写的基类加载器，所以还有待完善，增强兼容性。注意一下，当返回一个const&时(常量引用)，必须要使用常量引用
来接收时，才不会创建对象副本。否则仍然会调用拷贝构造函数来创建对象副本。如果使用普通引用(非常量)来接收，则编译会报错。	<br>

## 2019.1.30 <br>
添加Camera组件，并随之往shader中添加了ViewMatrix和ProjectionMatrix(关于这两个矩阵如何传入，后面还要修改)。完善了BaseEntity的一些方法，理清了场景树(由TransformSystem来控制)，
后面将对这个组件进行更具体的修改，如添加四叉树或者八叉树的管理来优化物体的可见性检测等。这里要注意BaseEntity中的MARK_SPECIAL_COMPONENT函数，其中用到了一个指针引用
(T* &)的参数，来给成员变量赋值，注意这里一定要是引用才可以，不然成员变量的指针仍然无法被赋值。后面对于多次调用的dynamic_cast类型转换，尝试在初始化时赋值，减少转换开销。
另外Model矩阵后面也需要传入shader，真正的利用起Transform系统。并且从现版本开始，正式使用Refrence来自动释放BaseComponent，当使用component时，需多加注意！！     <br>

## 2019.1.31 <br>
丰富了Entity的接口，修改了部分渲染逻辑，传入了model矩阵，真正的串起了之前的所有系统。另外添加了代码根目录进头文件路径，以后不需要".."或者"."这样写一大堆点了，后面会集中修改。
接下来会添加模型的读取和模型cache的封装等，然后开始使用模型文件来进行渲染。    <br>

## 2019.2.2~2019.2.3 <br>
封装了Model类用来处理模型j加载,后面还将加入cache机制。在实现过程中,忘了设置开启opengl的深度测试,导致显示结果不正确(不开启深度测试,后渲染的片段都会覆盖之前的片段)。另外,在模型
加载多个纹理的时候,用到了lambda表达式。这里需要特别注意,当lambda表达式按值捕获变量的时候,会生成一份该对象的拷贝,所以lambda表达式内部修改他的值,并不影响外部,原对象呗销魂也
不会影响lambda内部的对象。而按引用捕获对象时,需要注意对象的生命周期,避免传入一个局部变量,这个对象的引用在内部被修改时,外部的对象也会相应的被修改。因为assimp加载的时候如果用
到了参数aiProcess_FlipUVs就会为我们翻转uv,但我们image里面已经处理过纹理的翻转,所以这里并没有使用,为了后边通用(平面ui仍然会发生颠倒)。   <br>

## 2019.2.4 <br>
添加了ModelCache,修复模型加载的一些问题。之前对纹理绑定的理解有些问题,纹理绑定病不是绑定到shader对象上的,而是绑定到opengl的,所以不能为每个shader去保存纹理绑定缓存队列。现在为了保存这个状态
创建了GLStateCache类,GL的状态控制后面都可以用这个类来修改(如开启深度测试等)。  <br>

## 2019.2.7 <br>
为了方便调试,增加了模型坐标轴的显示,以及AABB包围盒的显示,后面需要根据AABB包围盒来做模型的可见性检测。后续要给entity增加接口,来控制包围盒和坐标轴的显示隐藏。并且现在所有顶点都是用VertexData
后面是否需要还需要思考,因为现在包含很多无用的数据,并不需要占这么多缓存,但这种情况只是少数用来做调试的,考虑到通用型以及合批,vertexData这块到底是否需要改进还是有必要好好考虑一下的。    <br>

## 2019.2.8 <br>
添加模型的可见性检测,并在代码中发现个几个大bug。1) AABB包围盒经过优化算法后,每帧计算出来的顶点位置,要加上的是模型在世界坐标系下的位置,而不是模型坐标系下的位置。2) Transform组件中,获取世界坐标
位置有一处理解错误。世界坐标应该等于父节点的model矩阵和当前模型坐标位置的乘积,而不是使用当前节点的model矩阵。3) Transform组件中,每帧计算节点model矩阵时,当将计算好的位移矩阵和旋转矩阵相乘时,
位移矩阵应该摆在左边,因为glm是列主序的矩阵,之前乘反了,导致模型显示的位置不正确!!。 经过这次的修改,Transform组件应该没有太大的问题了,后面需要针对计算这块优化一下,减轻每帧的计算工作量。  <br>

## 2019.2.9~2019.2.13 <br>
添加渲染路径的选择，目前还没具体实现延迟渲染。修复了AABB包围盒之前一直存在的bug，多乘了一次model矩阵，其本身已经是在世界坐标系下的坐标了，所以在可见性检测中转到裁剪坐标系是不需要再乘model
矩阵的。另外，win下会默认垂直同步开启，造成显示的fps一直维持在60帧左右，关于关闭垂直同步的相关代码还有些问题，后面还需要好好看看。	<br>

## 2019.2.16 <br>
pc下渲染顶点变多，会造成fps严重下降，当渲染60多万个顶点的时候每秒只有十几帧。部分原因是因为每帧都会将mesh的顶点数据一个一个传给vbo（从cpu到gpu），这种方式是从cocos中抄的，但cocos是一款2d
引擎，顶点数很低。这种方式完全不适合3D引擎的渲染，所以现在暂时先为每个mesh都保存一个vbo，并在生成的时候将数据传进gpu。但现在这并不是最好的解决方案：要考虑顶点的位移和动画，另外要考虑多
个mesh的合并（顶点数很少的mesh合并到一起）。例如6万个顶点左右合并到一个mesh中，用来做动态合批，并且这些mesh使用同一个model、TBN或是骨骼等矩阵。后面需要将entity分为static渲染和dynamic渲
染两类，并且给Transform和可见性检测都加上脏标记，当模型未发生改变时，不去计算。 <br>

## 2019.2.19 <br>
发现另外一个导致帧率低的原因是glUniformXXX造成的，由于shader是重复使用的，所以uniform每帧都在重新从cpu传值给gpu，而且是将GLProgram的map中所有uniform参数都重新传一遍，这造成了极大的浪费。
可以考虑将没有变化的参数，不重新传值；也可以将需要多次使用的shader多创建几份，这样牺牲显存来减少cpu到gpu的io次数。后面需要好好考虑渲染这块的框架，会先完成动画部分。<br>

## 2019.2.21~2019.2.23 <br>
接入ImGui，用来做数据显示面板，面板这块还需要继续封装，先理了一份大概的架构，后面会继续优化。注意Transfrom的EulerAngle欧拉角属性，因为使用了yaw-pitch-roll，所以pitch也就是绕x轴旋转，必须控制在
[-90, 90]的范围内。所以单独为超过范围的欧拉角设置一下值，这样才能保证欧拉角的唯一性。unity这块不知道是如何处理的，它是可以填超过这个范围的数值的。另外针对数值变化回调判断这块，需要优化一下。 <br>

## 2019.2.24~2019.2.25 <br>
添加Hierarchy面板和GameStatus面板，用来显示场景树和游戏状态。之前异步加载没有判断正在加载的资源，会出现内存泄漏，并有可能导致指针失效，这次
修复了这个问题。另外在实体被销毁的时候，需要删除Hierarchy里的数据，后面要记得处理一下，对动态生成的资源，可能会出现空指针的情况，一定要处理好。
下面会开始添加模型动画，这段时间接入ImGui也是为了方便动画的阅览和调试，个人感觉对Component在Inspector面板中显示的处理支持的还不错。<br>

## 2019.2.26~2019.3.3 <br>
为模型增加了骨骼信息读取，添加了Animator组件和动画系统，还没有测试是否正确。对于动画相关的骨骼变换矩阵，需要从模型的根节点处去处理，所以动画
组件也是加在模型根节点处的，因为模型动画中的aiAnimNode** mChannel需要找到对应的aiNode节点，计算每个节点的本地变换后，再从根节点开始遍历，计算
一遍所有节点的骨骼坐标系变换矩阵。现在虽然将顶点变换了，但之前改了RenderSystem造成顶点没有传给gpu，所以还没看到结果，不知道实现是否有问题。<br>
另外对于c++的容器resize和reserve的不同点：调用resize()函数后，所有的空间都已经初始化了，所以可以直接访问。
而reserve()函数预分配出的空间没有被初始化，所以不可访问。调用resize(n)后，容器的size即为n。至于是否影响capacity，取决于调整后的容器的size是否大于capacity。
调用reserve(n)后，若容器的capacity<n，则重新分配内存空间，从而使得capacity等于n。如果capacity>=n呢？capacity无变化。<br>

## 2019.3.4~2019.3.6 <br>
完成了骨骼动画的播放，包含两种模式：使用CPU播放和使用GPU播放。前面计算骨骼矩阵(需要遍历node的树)和计算蒙皮矩阵有问题，所以导致骨骼动画播放不正确。
另外，当使用gpu播放动画时，即要给gpu传递影响每个顶点的骨骼id数组和骨骼权重数组，骨骼id数组需要通过glVertexAttribIPointer来绑定int类型的数组，
glVertexAttribPointer只能绑定浮点型的，调试中因为错误使用造成没有索引到指定的骨骼矩阵。后面需要对系统进行整理，代码优化，减少不同组件之间的耦合。
对于动画系统，还需要实现动画的融合，并查询顶点动画相关的知识，看看能不能同时支持顶点动画和骨骼动画。骨骼动画播放起来性能很差，要好好优化。 <br>

## 2019.3.7 <br>
优化骨骼动画，骨骼矩阵的最后一行一直是(0, 0, 0, 1)，所以可以直接用一个3x4的矩阵来代替mat4，减少数据的传递。

## 后续工作 <br>
0）骨骼的显示，包围盒的旋转问题，GLProgram的缓存机制问题，实体Visible的问题，骨骼动画的优化。优化重构代码，减轻组件之间的耦合，看看ogre中如何实现和优化的。<br>
1）MeshFitter的AABB包围盒的生成,以及可见性检测。关于可见性检测,可能会需要添加四叉树或者八叉树来管理场景 <br>
2)  模型动画的读取和处理,区分骨骼动画和顶点动画(unity可以自己生成骨骼,不太理解他的骨骼的生成是否需要骨骼动画来支持) <br>
3)  OpenGL不同状态的处理,深度测试,模板测试, 面剔除等(考虑到有这些状态,似乎命令模式时最好的选择,只需在相应的位置插入命令即可。unity是在他自己的shader中通过设置标签等属性来控制这个的,也可以考虑这样处理。) <br>
4)  文字的显示处理(freetype库?) <br>
5)  事件系统(包括键盘,鼠标事件和普通的用户自定义等)<br>
6）引擎更新的帧率，sleep，以及对deltaTime的计算<br>
7）Custom（用户逻辑层面）等<br>
8）各种工具方法的完善<br>
9）通过引用计数，看看是否可以写出一个通过引用计数来做内存分析的Profile工具类<br>
10）实现延迟渲染，以及Light和阴影等。<br>
11）多级渐远纹理的实现。<br>
12）对VAO生成和mesh合批的处理(是否可以将mesh作为最基本的渲染单元)。 <br>
13）内存的管理与检查。 <br>
等等<br>
