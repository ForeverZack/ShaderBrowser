# ShaderBrowser

## 后续工作 <br>
-4)阴影(Shadow Map(SM),Cascaded Shadow Map(CSM: 级联阴影);延迟渲染考虑一下?)
-3)sharedMesh的实现，个人理解是MeshFilter的Mesh是shared类型的，而SkinnedMeshRenderer是非share的(或者可以通过useGPU来判断)，
-1) 切分动画，以及切分动画之间的混合等操作 <br>
0）骨骼的显示，包围盒的旋转问题，GLProgram的缓存机制问题，实体Visible的问题，骨骼动画的优化(例如看不见的模型，是不需要更新计算骨骼矩阵的)。优化重构代码，减轻组件之间的耦合，看看ogre中如何实现和优化的。<br>
1）MeshFitter的AABB包围盒的生成,以及可见性检测。关于可见性检测,可能会需要添加四叉树或者八叉树来管理场景 <br>
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

## 2020.5.15~2020.5.20 <br>
添加RenderTexture(还没正式使用过)；丰富Camera的属性，使用Depth来对Camera的渲染顺序进行排序，并允许多个Camera被渲染。
(允许被渲染的相机必须符合以下规定，相机的渲染纹理不为空的相机；渲染纹理为空的相机列表中Depth最大的相机。) <br>

## 2020.5.2 <br>
解决了fighter骨骼动画不正确的问题，原因是assimp生成的preserve pivots是会发生变换的，之前在使用Transform组件的变换来计算
骨骼的变换，忽略了这个preserve pivots。现在将其算作骨骼(并非原始fbx的骨骼)。<br>

## 2020.4.21~2020.4.23 <br>
之前使用新的材质替换原来的材质时，会因为transform和camera的dirty标记不正确，造成模型无法显示。
材质文件现在基本完成并可以使用，后面可以进一步完善。添加了fighter的法线贴图。<br>

## 2020.4.18~2020.4.20 <br>
添加材质文件(格式规定(json)、解析、加载)，现在已经可以将材质文件还原出材质对象，后面将正式使用起来。
升级imgui到1.76版本。<br>

## 2020.3.29 <br>
### 注意一下 <br>
mac上第一次绘制的时候会非常卡，是SkinnedRenderCommand在glDraw的时候会非常耗时，但不清楚为什么(不使用点光源或者注释掉
点光源的Color设置就会正常。但尝试了将shader中所有光照部分的代码移除，仍然很耗时，后面有机会要好好看看！！)。今天新增
了rapidjson库，准备后面用来解析引擎自己的材质(顺带可以干掉mesh中存的纹理数据)。<br>

## 2020.3.20~2020.3.22 <br>
升级imgui到最新的1.75版本，处理了imgui的多线程使用问题。最初是准备创建多个imgui上下文来给每个逻辑帧使用，再在渲染帧
中去取这个上下文绘制，虽然这样可以正常显示，但imgui缓存的输入数据会变的不正确，例如InputTextState需要复制前一帧的数据，
不然输入框会一直认为数据在改变。因为出了InputTextState以外，imgui还缓存了其他的交互数据，所以这样做肯定并不保险，也不
方便后面对imgui进行升级，所以现在还是在全局只使用一个imgui的上下文。但这个上下文的数据可能逻辑线程和渲染线程都不是最
新的，会有一定延迟，但好在这里ui的即时性要求并没有那么强烈。<br>

## 2020.3.14~202.3.16 <br>
添加聚光灯，采用和Unity一样的衰减纹理贴图(距离衰减+广角衰减)。发现Model加载必须要加回调，否则会不正确，需要检查一下。
后面可以开始着手阴影处理了。 <br>

## 2020.3.7~2020.3.8 <br>
添加点光源，点光源的衰减计算采用和unity一样的衰减纹理贴图。现在资源加载完成之后有一些卡顿，需要检查一下。 <br>

## 2020.3.2 <br>
之前的高光反射看上去不对是因为模型的发现没有标准化，标准化之后看上去正常了，但specColor和gloss现在是写死的，后面要传入。<br>

## 2020.3.1 <br>
加入BlinnPhong光照模型，但高光反射看上去好像不太对。<br>

## 2020.2.29 <br>
整理优化光照系统代码结构，现在光照系统既可以在脏标记为true时仅更新已有的材质，也可以对新增材质设置所有光照属性。
另外新增shader通用方法，给静态模型加入兰伯特光照和半兰伯特光照模型；并将所有顶点和片段着色器的通用结构体封装进
CommonStruct.inc中，简洁shader代码。 <br>

## 2020.2.25 <br>
为所有材质设置了平行光属性。为了性能考虑还是保留了光照系统的脏标记，这部分脏标记会更新所有已有的材质光照；对
新增的材质，会为它们赋值所有的光照属性。但这样写会让光照系统中有两份类似的代码，很难受。特别注意一定要保证所有
异步加载回调可能创建材质的地方，都要放到光照系统刷新前执行，否则会赋值不到新增材质。 <br>

## 2020.2.22~2020.2.23 <br>
加入光照系统，还没做完。之前的想法是把材质灯光属性的更新放到RenderCommand中去，但这样不显示的模型材质会更新不到，
应该在光照系统中刷新时，对所有material都设置一遍灯光属性(可以考虑shader中有无光照来做一些优化)。<br>

## 2020.2.20~2020.2.21 <br>
为shader添加include机制，方便将公共方法移入单独的文件，例如使用到标准变量的部分放入了Standard.inc中、光照放入Light.inc中。<br>

## 2020.2.19 <br>
由于拆分了逻辑线程和渲染线程，但imgui里获取鼠标焦点是否在窗口内一直是false，所以imgui始终无法获取io的鼠标位置，现在去掉了
这个焦点判断，可以正常使用了。<br>

## 2020.2.16~2020.2.17 <br>
整理代码，提高线程之间的通讯效率，现在pc上帧率也有所提升：
1.逻辑线程的状态控制并不需要MutexUnorderedMap，只需要记录一下已经计算到第几帧即可；<br>
2.之前Animator会使用三个unordered_map来保存骨骼的变换信息，多了一次存储和对骨骼Transform赋值的过程，现在直接对骨骼Transform
赋值。但这样做的前提是TransformSystem已经完成计算，所以TransformSystem无法再使用另一个线程和AnimationSystem同时进行计算了；<br>
3.BaseThreadPool的active判断似乎有些延迟，所以在AnimationSystem中增加了对组件计算完成数量的统计，将它和线程池的isActive
结合起来使用；<br>

## 2020.2.14~2020.2.15 <br>
给逻辑线程增加了队列(包括逻辑线程状态队列，GPU资源命令队列，渲染命令队列)，让其可以领先渲染线程3帧(3帧之后才会等待)。
去掉了之前逻辑线程等待渲染线程的部分。<br>

## 2020.2.9 <br>
pc下帧数并没有明显提升，而且变得不稳定，怀疑可能是逻辑线程和渲染线程中都加锁相互限制了。<br>

## 2020.1.29 <br>
解决之前偶尔会出现模型不显示的问题，是渲染线程和逻辑线程在RenderSystem中使用了同一个队列造成的。
后面应该考虑尽可能的减少线程之间的相互等待。<br>

## 2020.1.28 <br>
将输入操作移到逻辑线程，添加Input类（渲染线程应该没有获取输入的需求）。

## 2020.1.27 <br>
解决昨天的部分bug(按序号来解释)。<br>
2.程序运行有时不会出现窗口是部分逻辑错误造成的，直接把主线程跳出了循环结束了。<br>
3.glfwTerminate需要在主线程调用，而不是渲染线程。<br>
今天也出现了新的问题。<br>
1.偶尔会出现模型不显示的问题(命令队列里这些东西都是存在的，可能是GPU资源指令创建设置的时候出了什么bug)。<br>

## 2020.1.25~2020.1.26 <br>
分离出渲染线程，这个过程中碰到很多问题以及一些思考。<br>
1.mac上glfw的初始化、创建窗口以及事件处理(这部分后面要独立出来)必须在主线程完成，具体信息参考Documents/Multithreading GLFW_ - support - GLFW.mhtml（大概就是说Cocoa只有主线程才能接收到窗口的事件队列）。然后逻辑线程和渲染线程分别有自己的状态，它们会相互等待（例如渲染线程
会等待逻辑线程将一帧的数据处理完毕，才开始渲染；而逻辑线程会等待渲染线程渲染完上一帧，才更新窗口并进入下一个循环的等待状态）；<br>
2.程序有时候不出现窗口，直接退出，原因有待查询；<br>
3.关闭窗口会报错，有待解决；<br>
4.事件的处理，这一点上面已经提到过了，要在主线程处理，并将消息分发出去；<br>
5.渲染线程中，不能处理逻辑线程的资源加载(如GLProgram)，并且更新缓冲的clear color要放在处理命令队列之前，而不是在渲染系统刷新中（现在渲
染系统的刷新是在逻辑线程中进行的，它只负责生成命令队列）；<br>
6.逻辑线程会负责glfwPollEvents，而这里需要等待渲染线程完成上一帧的渲染；<br>
总体来说，渲染线程的分离大大提高了帧数（至少现在在mac上是这样，后面用pc跑跑看），上述问题和代码结构还需整理解决！！！

## 2020.1.24 <br>
整理RenderSystem，把生成命令队列和绘制操作分开。这里分开之后，就没有了之前开始第一帧会出现画面撕裂的问题了。<br>

## 2020.1.1 <br>
修复Mesh的GPU操作命令问题，是因为把vao的索引数组缓冲绑定到0了(其实glBindBuffer没有必要重置为0)。至此，目前拥有
GPU资源的操作命令基本都已完工，也都用上了，后面可以好好整理一下代码，并开始着手实现渲染线程分离了。 <br>

## 2019.12.28～2019.12.29 <br>
修复纹理显示问题，是忘了将模型的Texture设置成GL_REPEAT造成的。之前的逻辑顺序和假设都没有问题，pc下异步加载
偶尔会报错，需要检查。修改Mesh，添加Mesh操作命令，还存在问题，需要慢慢找。 <br>

## 2019.12.19 <br>
将GPU操作命令的更新放到一起执行。Model的异步加载有问题，不可以直接去创建(因为会创建Material，命令顺序有问题)
，现在GLProgram的创建和setUniform的顺序有问题，Model异步应该只加载数据，创建还是在逻辑线程进行。<br>

## 2019.12.13~2019.12.15 <br>
添加纹理缓存，替代Animator中的tbo <br>

## 2019.12.9~2019.12.10 <br>
修改渲染命令、Material以及Pass，整理完善GLProgram的uniform参数传递。后面还要处理mesh数据的修改以及骨骼矩阵
的传递。注意，标准库容器如果是复制的，要记得写对象的拷贝构造函数，后面很多数据都要通过由逻辑线程复制传给渲
染线程的。<br>

## 2019.12.6~2019.12.8 <br>
整理资源结构，将GPU资源独立出来。添加GPU相关操作指令(目前只有Texture2D和GLProgram)，现在这些指令还是在逻辑
线程里执行的，等后面GPU操作指令都写完，并保证渲染指令没问题(渲染指令要保存的是GPU资源相关的属性，而非逻辑线
程中的资源属性)，才可以拆分逻辑线程和渲染线程。<br>

## 2019.11.9 [版本 release v1.0](https://github.com/ForeverZack/ShaderBrowser/releases/tag/v1.0) <br>
为了后面能把渲染线程独立出来，现在大致将结构修改了一下，分为逻辑核心和渲染核心，并用Application将他们串联起来
后面将对所有的渲染相关的操作进行封装，方便后面将他们全部挪到渲染线程中去。<br>

## 2019.10.20 <br>
将TranformSystem的update放到最前面，骨骼动画造成的节点的变换由AnimationSystem来算（多线程），并将TransformSystem
的计算放入一个线程当中（需要注意骨骼Transform和场景静态Transform的依赖关系，即计算的先后顺序，有依赖关系的需要等其
计算完才可以使用）。恢复了applyRootMotion，后面还需要做混合。另外改了一下结构，所有的AddComponent事件必须
要有component自身，才能不限制组件的添加顺序。用多线程去处理BoundBox（不等待它全部执行完成，只需要分一
些线程去跑即可，不会很精确，会有一定的延时但应该够用），方便后面做视锥体剔除和遮挡剔除。<br>

## 2019.10.13 <br>
之前通过是uniform mat3x4的数组来传递骨骼矩阵的，这种方式需要将矩阵一个个传入，效率较低。现在将其改为tbo的方式，
采用samplerBuffer传入，占用一个纹理单元，但传输速度有很大的提升，win下的帧数有明显的改善，后面还需要看看哪些地方
有可以优化的地方。现在880m大概能跑40～50帧，接近原来的2倍。 <br>

## 2019.9.23~2019.10.6 <br>
加入了线程池(实现了对线程的管理，具体可以查看ThreadPool的说明，踩了不少坑，总算是把线程的休眠唤醒和销毁都处理好了)。
注释掉了之前对于骨骼动画优化的处理(Transform Feedback，Compute Shader，序列帧)，改成用多线程来计算骨骼动画，并将
之前的加载器的多线程部分，统一替换成线程池(线程增加了，加载速度也上来了)。 <br>

## 2019.9.14 <br>
修复之前骨骼动画转序列帧的问题，并将setQuaternion函数修改，提升效率。现在帧数有一定提升，但初始化时转换的过程在win
上非常耗时，考虑后面做离线处理。    <br>

## 2019.9.8 <br>
将骨骼动画转为序列帧的数据,但实现跑起来还有些问题需要检查 <br>

## 2019.8.11 <br>
之前添加计算着色器时，由于要获取纹理单元的值，所以加了一个getTexUnit的方法，但是这个方法必须让opengl使用该着色器之后
才可以调用。	<br>

## 2019.7.29~2019.8.10 <br>
实现了ComputeShader，想尝试用它来计算骨骼动画会不会更快，但还有些问题需要修改。另外mac系统最高只支持OpenGL4.1(Compute
Shader需要OpenGL4.3)，所以mac上无法使用计算着色器，需要做好区分！！！<br>

## 2019.7.23~2019.7.26 <br>
实现了Tranform Feedback来计算骨骼动画的骨骼节点变换，但有些动画的变换还不太正确，需要修改。另外Transform Feedback实现骨骼
动画的效率很低，还没有cpu高。但unity计算骨骼动画的效率就很高，应该还有很大的提升的空间<br>

## 2019.7.22 <br>
win32下对TransformFeedback的兼容处理，使用了核心模式430版本，并且win32中shader不能直接使用samplerBuffer作为函数的变量类型，
shader会无法编译过去。另外现在动画系统中的Transform Feedback效率很低，并没有直接用cpu计算的快，需要查找原因。 <br>

## 2019.7.15～2019.7.16 <br>
将骨骼矩阵的计算放到Transform系统中去执行，放弃原来对整个模型计算骨骼的方式。这样做有几点需要说明，之前绑定网格模型时，由于
骨骼矩阵计算是在根结点进行的，所以之前要把网格模型的父节点转为模型根结点，但现在不需要；现在不支持
AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS，具体原因尚不清楚，似乎是有些节点的变换没有传进来；另外Transform的脏标记对骨骼矩阵
的更新似乎有问题，后面需要具体查一下。将骨骼矩阵的计算放到Transform系统的好处就是，后面将可以使用TransformFeedback特性来更新
骨骼动画。 <br>

## 2019.7.14 <br>
考虑到opengl默认使用了双缓冲，所以Transform Feedback也需要对应的双缓冲处理。 <br>

## 2019.7.13 <br>
增加将矩阵拆解到位移、旋转和缩放的方法，为后面重构骨骼动画做好理论准备。之前的骨骼动画的计算没有使用到渲染引擎中的Transform类
，造成播放骨骼动画的模型，没法通过transform进行移动。另外，之前的结构也不利于使用Transform Feedback来回传骨骼动画的信息。
后面将进行重构，将骨骼的变换设置到对应的Transform上，骨骼矩阵boneMatrix也将通过Transform系统来计算，而非现在单独计算。<br>

## 2019.7.10～2019.7.13 <br>
让TransformFeedback支持GL_SEPARATE_ATTRIBS模式，可以使得各个buffer之间分开。之前在读取回传的buffer时遇到了一些问题，原因
在于这些buffer在创建的时候其实是GL_ARRAY_BUFFER或GL_TEXTURE_BUFFER并被绑定到GL_TRANSFORM_FEEDBACK_BUFFER对应
的绑定位置bindIdx上，但读取的时候仍然需要按照创建buffer时的类型来读取，不能以GL_TRANSFORM_FEEDBACK_BUFFER类型来读取。<br>

## 2019.7.8～2019.7.9 <br>
优化健全Transform Feedback的基类，添加对纹理缓存(Texture Buffer Object的支持)，丰富了GLProgram的uniform类型的接口，为后面在
顶点着色器中计算骨骼动画做好准备。 <br>

## 2019.7.6~2019.7.7 <br>
添加Transform Feedback，后续将用来计算骨骼矩阵。由于骨骼节点之间是有父子关系的，子级节点想要得到自己的骨骼矩阵，就必须先得到
父节点的骨骼矩阵，但gpu中的shader是并行的，所以这里暂时只能想到的做法是，把当前骨骼节点的父节点全部遍历计算一遍。好在骨骼的
数量并不多，而且是在顶点着色器内计算，所以消耗应该还可以。这样的做法需要很多数据的支持，等于整个骨骼动画的计算都在gpu中，需要
好好考虑所需的数据，避免多次重复传输。 <br>

## 2019.6.30 <br>
添加了FlatBuffer库，后面将用来存储读取二进制文件（如纹理动画数据等）。<br>

## 2019.4.29~2019.5.4 <br>
拆分了VertexData (这样做效率有所下降，但使用内存空间降低，也更灵活)。添加了渲染命令，后面需要再对这些渲染命令进行排序和合并操作。
另外，拆分过后BaseEntity::getVertices需要修改，法线和切线目前还没有传。<br>

## 2019.4.21 <br>
关闭Mac下的垂直同步。添加MaterialCache(针对SharedMaterial)，初步整理了一下材质相关的结构，Renderer中的材质采用了vector，后面将支持多个材质的渲染。另外，
对一个节点存在多个Mesh的情况，简单的观察了一下unity的处理方式，应该是mesh队列和material队列是一一对应的，超出的material将使用最后
一个mesh来进行重复的渲染工作。 <br>

## 2019.4.14 <br>
整理一些之前对渲染组件理解的错误，顺便理一下渲染架构重写的思路，后面会对这块整个重写的。小优化了一下UniformValue，加了个dirty标记。 <br>

## 2019.4.13 <br>
修复读取模型文件时，网格模型绑定到骨骼后位置不正确。原因是读取的模型文件，如果普通网格模型默认绑定在骨骼节点上，网格模型的原点是整个模型的原点，所以
顶点位置会不正确，这时需要将模型空间的顶点转换到骨骼节点空间下，因为现在程序已经为每个骨骼节点单独生成了Transform组件，并会实时更新它。后面做Animation
Instance的时候要注意这块。之前从来没有注意过aiBone->mOffsetMatrix的属性，其实mOffsetMatrix很重要，是用来将网格中模型空间的顶点转换到骨骼空间中，变换到
骨骼空间后才可以使用骨骼矩阵！！！ <br>

## 2019.4.6～2019.4.7 <br>
添加SkinnedMeshRenderer，与原来的BaseRender做区分(实际上就是将MeshFilter,BaseRender,BoundBox合并到一起)。还有一些问题：运行时可能会看不到某个模型；
绑定网格模型到骨骼时位置不正确。另外，Animator::BlendTo也需要记录一下骨骼节点的Transform信息，现在只给Animator::play加了。<br>

## 2019.4.5 <br>
添加骨骼动画的混合操作，支持从一个动画混合播放到另外一个动画。并且添加了Apply Root Motion属性，用来控制模型骨骼根节点的移动数据是否有用 (有的    
模型根节点会有移动动画，有时候这个会不符合我们的需求)。 <br>

## 2019.3.30~2019.3.31 <br>
修复模型信息读取时骨骼的重复创建(之前的写法认为每个aiMesh之间的骨骼没有联系，这实际上是不对的，不同的aiMesh会共用一套骨骼，所以在整理骨骼信息时
要去除相同的部分)。另外，添加了网格模型绑定到单个骨骼的操作，例如资源中unity官方的模型RawMocap的眼睛是直接挂在骨骼节点下的，但模型信息中并没有
直接将其绑定到某根骨骼。所以在读取模型时，添加了对网格模型的二次检查，确认其是否被直接挂在某个骨骼节点下，如果是并且其信息中没有被绑定到任何骨骼
上，则会执行绑定骨骼的操作。后面还需要用更多的模型做测试，并且控制模型动画不在原点移动的问题。 <br>

## 2019.3.21 <br>
修复了Inspector面板输入欧拉角会因为Transform组件内部计算存储的欧拉角的值不同而造成循环调用的情况(增加了一个专门用来在Inspector面板中显示的变量)。 <br>

## 2019.3.7 <br>
优化骨骼动画，骨骼矩阵的最后一行一直是(0, 0, 0, 1)，所以可以直接用一个3x4的矩阵来代替mat4，减少数据的传递。<br>

## 2019.3.4~2019.3.6 <br>
完成了骨骼动画的播放，包含两种模式：使用CPU播放和使用GPU播放。前面计算骨骼矩阵(需要遍历node的树)和计算蒙皮矩阵有问题，所以导致骨骼动画播放不正确。
另外，当使用gpu播放动画时，即要给gpu传递影响每个顶点的骨骼id数组和骨骼权重数组，骨骼id数组需要通过glVertexAttribIPointer来绑定int类型的数组，
glVertexAttribPointer只能绑定浮点型的，调试中因为错误使用造成没有索引到指定的骨骼矩阵。后面需要对系统进行整理，代码优化，减少不同组件之间的耦合。
对于动画系统，还需要实现动画的融合，并查询顶点动画相关的知识，看看能不能同时支持顶点动画和骨骼动画。骨骼动画播放起来性能很差，要好好优化。 <br>

## 2019.2.26~2019.3.3 <br>
为模型增加了骨骼信息读取，添加了Animator组件和动画系统，还没有测试是否正确。对于动画相关的骨骼变换矩阵，需要从模型的根节点处去处理，所以动画
组件也是加在模型根节点处的，因为模型动画中的aiAnimNode** mChannel需要找到对应的aiNode节点，计算每个节点的本地变换后，再从根节点开始遍历，计算
一遍所有节点的骨骼坐标系变换矩阵。现在虽然将顶点变换了，但之前改了RenderSystem造成顶点没有传给gpu，所以还没看到结果，不知道实现是否有问题。<br>
另外对于c++的容器resize和reserve的不同点：调用resize()函数后，所有的空间都已经初始化了，所以可以直接访问。
而reserve()函数预分配出的空间没有被初始化，所以不可访问。调用resize(n)后，容器的size即为n。至于是否影响capacity，取决于调整后的容器的size是否大于capacity。
调用reserve(n)后，若容器的capacity<n，则重新分配内存空间，从而使得capacity等于n。如果capacity>=n呢？capacity无变化。<br>

## 2019.2.24~2019.2.25 <br>
添加Hierarchy面板和GameStatus面板，用来显示场景树和游戏状态。之前异步加载没有判断正在加载的资源，会出现内存泄漏，并有可能导致指针失效，这次
修复了这个问题。另外在实体被销毁的时候，需要删除Hierarchy里的数据，后面要记得处理一下，对动态生成的资源，可能会出现空指针的情况，一定要处理好。
下面会开始添加模型动画，这段时间接入ImGui也是为了方便动画的阅览和调试，个人感觉对Component在Inspector面板中显示的处理支持的还不错。<br>

## 2019.2.21~2019.2.23 <br>
接入ImGui，用来做数据显示面板，面板这块还需要继续封装，先理了一份大概的架构，后面会继续优化。注意Transfrom的EulerAngle欧拉角属性，因为使用了yaw-pitch-roll，所以pitch也就是绕x轴旋转，必须控制在
[-90, 90]的范围内。所以单独为超过范围的欧拉角设置一下值，这样才能保证欧拉角的唯一性。unity这块不知道是如何处理的，它是可以填超过这个范围的数值的。另外针对数值变化回调判断这块，需要优化一下。 <br>

## 2019.2.19 <br>
发现另外一个导致帧率低的原因是glUniformXXX造成的，由于shader是重复使用的，所以uniform每帧都在重新从cpu传值给gpu，而且是将GLProgram的map中所有uniform参数都重新传一遍，这造成了极大的浪费。
可以考虑将没有变化的参数，不重新传值；也可以将需要多次使用的shader多创建几份，这样牺牲显存来减少cpu到gpu的io次数。后面需要好好考虑渲染这块的框架，会先完成动画部分。<br>

## 2019.2.16 <br>
pc下渲染顶点变多，会造成fps严重下降，当渲染60多万个顶点的时候每秒只有十几帧。部分原因是因为每帧都会将mesh的顶点数据一个一个传给vbo（从cpu到gpu），这种方式是从cocos中抄的，但cocos是一款2d
引擎，顶点数很低。这种方式完全不适合3D引擎的渲染，所以现在暂时先为每个mesh都保存一个vbo，并在生成的时候将数据传进gpu。但现在这并不是最好的解决方案：要考虑顶点的位移和动画，另外要考虑多
个mesh的合并（顶点数很少的mesh合并到一起）。例如6万个顶点左右合并到一个mesh中，用来做动态合批，并且这些mesh使用同一个model、TBN或是骨骼等矩阵。后面需要将entity分为static渲染和dynamic渲
染两类，并且给Transform和可见性检测都加上脏标记，当模型未发生改变时，不去计算。 <br>

## 2019.2.9~2019.2.13 <br>
添加渲染路径的选择，目前还没具体实现延迟渲染。修复了AABB包围盒之前一直存在的bug，多乘了一次model矩阵，其本身已经是在世界坐标系下的坐标了，所以在可见性检测中转到裁剪坐标系是不需要再乘model
矩阵的。另外，win下会默认垂直同步开启，造成显示的fps一直维持在60帧左右，关于关闭垂直同步的相关代码还有些问题，后面还需要好好看看。    <br>

## 2019.2.8 <br>
添加模型的可见性检测,并在代码中发现个几个大bug。1) AABB包围盒经过优化算法后,每帧计算出来的顶点位置,要加上的是模型在世界坐标系下的位置,而不是模型坐标系下的位置。2) Transform组件中,获取世界坐标
位置有一处理解错误。世界坐标应该等于父节点的model矩阵和当前模型坐标位置的乘积,而不是使用当前节点的model矩阵。3) Transform组件中,每帧计算节点model矩阵时,当将计算好的位移矩阵和旋转矩阵相乘时,
位移矩阵应该摆在左边,因为glm是列主序的矩阵,之前乘反了,导致模型显示的位置不正确!!。 经过这次的修改,Transform组件应该没有太大的问题了,后面需要针对计算这块优化一下,减轻每帧的计算工作量。  <br>

## 2019.2.7 <br>
为了方便调试,增加了模型坐标轴的显示,以及AABB包围盒的显示,后面需要根据AABB包围盒来做模型的可见性检测。后续要给entity增加接口,来控制包围盒和坐标轴的显示隐藏。并且现在所有顶点都是用VertexData
后面是否需要还需要思考,因为现在包含很多无用的数据,并不需要占这么多缓存,但这种情况只是少数用来做调试的,考虑到通用型以及合批,vertexData这块到底是否需要改进还是有必要好好考虑一下的。    <br>

## 2019.2.4 <br>
添加了ModelCache,修复模型加载的一些问题。之前对纹理绑定的理解有些问题,纹理绑定病不是绑定到shader对象上的,而是绑定到opengl的,所以不能为每个shader去保存纹理绑定缓存队列。现在为了保存这个状态
创建了GLStateCache类,GL的状态控制后面都可以用这个类来修改(如开启深度测试等)。  <br>

## 2019.2.2~2019.2.3 <br>
封装了Model类用来处理模型j加载,后面还将加入cache机制。在实现过程中,忘了设置开启opengl的深度测试,导致显示结果不正确(不开启深度测试,后渲染的片段都会覆盖之前的片段)。另外,在模型
加载多个纹理的时候,用到了lambda表达式。这里需要特别注意,当lambda表达式按值捕获变量的时候,会生成一份该对象的拷贝,所以lambda表达式内部修改他的值,并不影响外部,原对象呗销魂也
不会影响lambda内部的对象。而按引用捕获对象时,需要注意对象的生命周期,避免传入一个局部变量,这个对象的引用在内部被修改时,外部的对象也会相应的被修改。因为assimp加载的时候如果用
到了参数aiProcess_FlipUVs就会为我们翻转uv,但我们image里面已经处理过纹理的翻转,所以这里并没有使用,为了后边通用(平面ui仍然会发生颠倒)。   <br>

## 2019.1.31 <br>
丰富了Entity的接口，修改了部分渲染逻辑，传入了model矩阵，真正的串起了之前的所有系统。另外添加了代码根目录进头文件路径，以后不需要".."或者"."这样写一大堆点了，后面会集中修改。
接下来会添加模型的读取和模型cache的封装等，然后开始使用模型文件来进行渲染。    <br>

## 2019.1.30 <br>
添加Camera组件，并随之往shader中添加了ViewMatrix和ProjectionMatrix(关于这两个矩阵如何传入，后面还要修改)。完善了BaseEntity的一些方法，理清了场景树(由TransformSystem来控制)，
后面将对这个组件进行更具体的修改，如添加四叉树或者八叉树的管理来优化物体的可见性检测等。这里要注意BaseEntity中的MARK_SPECIAL_COMPONENT函数，其中用到了一个指针引用
(T* &)的参数，来给成员变量赋值，注意这里一定要是引用才可以，不然成员变量的指针仍然无法被赋值。后面对于多次调用的dynamic_cast类型转换，尝试在初始化时赋值，减少转换开销。
另外Model矩阵后面也需要传入shader，真正的利用起Transform系统。并且从现版本开始，正式使用Refrence来自动释放BaseComponent，当使用component时，需多加注意！！     <br>

## 2019.1.29 <br>
完善FileUtils，添加异步加载资源逻辑，因为是模板写的基类加载器，所以还有待完善，增强兼容性。注意一下，当返回一个const&时(常量引用)，必须要使用常量引用
来接收时，才不会创建对象副本。否则仍然会调用拷贝构造函数来创建对象副本。如果使用普通引用(非常量)来接收，则编译会报错。    <br>

## 2019.1.28 <br>
使用模板来创建单例和cache，加深了对模板的理解以及对c++编译链接的理解。通过模板，代码也变得更加精简。另外添加了文件管理系统，还没弄完，有待完善。    <br>

## 2019.1.18~2019.1.20 <br>
将MeshFilter拆分为多个Mesh。Mesh是封装顶点数据、纹理、VAO的最基础的单元，并对应一个Material的name。这些Material会在AddMesh或者给entity添加MeshFilter组件
的时候自动生成(Render组件和Material都只是容器，render的目的是根据mesh查找到对应的material，material则根据当前渲染队列找到对应的pass)。并添加了组件之间的
事件机制使不同组件之间可以相互通信，例如MeshFilter和Render组件之间。    <br>

## 2019.1.11~2019.1.13 <br>
封装了MeshFilter组件，整理优化了纹理的uniform，理顺了纹理单元的逻辑(需要用glUniform1i来对纹理单元进行绑定，并且只需要绑定一次即可)。在这期间，发现之前使
用unordered_map时，使用了const char*来作为它的key值，这样做会造成问题(需要自己去实现unordered_map的hash方法和比较方法)，所以现在已经全部替换为std::string
来作为key值。对于MeshFilter组件，后面会继续完善，实现从模型文件中获取顶点数据和纹理信息。<br>

## 2018.12.30~2018.12.31 <br>
完善Transform组件，实现了在逻辑阶段中获取设置组件属性的功能，与unity的结果完全相同。对于之前heading-pitch-bank旋转的问题，应该是glm是列主序的矩阵，需要
左乘，所以实际使用时顺序是z-y-x，所以得到的是一个 物体->惯性 的矩阵，后面将开始camera的封装，以及对模型的使用封装等。<br>

## 2018.12.30~2018.12.31 <br>
封装了Transform组件，但目前并不支持同一帧里去获得Transform的变换属性(因为这里都是在visit遍历的时候才计算)，后面还要继续优化。不过对于当中用到的Rotate方
法还有些疑问，虽然参照unity测试比较结果实现了，但对于model矩阵中的旋转矩阵的计算还是不太懂：Space.Self按照y-x-z的顺序旋转 和 Space.World按照z-x-y的顺序
旋转，得到的四元数为何都是 模型->惯性空间 的？？？？（个人认为是模型->惯性空间的依据，主要是因为使用了自己写的四元数转换欧拉角的函数，用到的是模型->惯性
空间的实现） <br>

## 2018.12.23 <br>
封装了顶点属性的结构，优化了RenderSystem的逻辑，现在改为使用索引的方式来绘制。并且可以对纹理进行采样，但对多级渐远纹理的处理还没有实现。概念上理清了VAO
和渲染组件的对应关系(对于VAO来说，只需要顶点数组属性结构相同，都可以使用相同的vao)，后面对于拥有相同的顶点属性结构的渲染对象来说，都可以使用同一个vao，
为往后的合批处理提供了方便。 <br>

## 2018.12.16 <br>
封装了uniform变量的结构体，修改了material的结构，在其中加入了pass，一个meterial能包含多个pass。不过目前只c支持执行第一个，后面在重写RenderSystem的时候会
考虑这块的逻辑。<br>

## 2018.11.20~2018.12.15 <br>
增加了对macOS的支持，加入了Assimp库，用于后续模型文件的读取<br>

## 2018.11.4~2018.11.9 <br>
封装了Texture2D，添加了引用计数机制 和 ESC框架，下面要完善ESC架构中的system和component。<br>


## 2018.10.28 <br>
封装渲染引擎架构，很多还有待完成。<br>
