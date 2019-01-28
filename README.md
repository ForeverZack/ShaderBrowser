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

## 后续工作 <br>
1）Transform(控制位置、缩放、旋转)、MeshFilter(保存网格顶点信息，但要处理好BaseRender的关系)、Custom（用户逻辑层面）等<br>
2）各种缓存机制，以及对缓存对象的引用计数控制（eg：ProgramCache, MaterialCache, TextureCache）<br>
3）引擎更新的帧率，sleep，以及对deltaTime的计算<br>
4）Camera类的实现<br>
5）3d模型的加载<br>
7）各种工具方法的完善<br>
8）各种基类的完善和内存控制<br>
9）通过引用计数，看看是否可以写出一个通过引用计数来做内存分析的Profile工具类<br>
10）实现延迟渲染，以及Light和阴影等。<br>
11）多级渐远纹理的实现。<br>
12）对VAO生成和materialID的生成的处理。 <br>
13）提取出BaseRender中的数据部分，独立出来看作是unity的MeshFilter组件，用这个来保管数据（这个数据应该是模型的原始数据，一个模型只用加载一次即可）。 <br>
14）异步加载资源的处理。 <br>
15）对内存的管理与检查。 <br>
等等<br>
