# shader库说明<br>
1.可以使用关键字#include "XXX"来在shader中使用后缀为inc的库(理论上支持任何后缀的文本)；<br>
2.被包含进来的库会在FileUtils的SearchPath列表中搜索；<br>
3.包含库文件之后，如果库文件中存在#ifdef或#ifndef，必须要先定义这些宏；<br>
4.可以使用[PreparePass]、[ShadowCaster]等标签，这些标签会根据材质的使用方式(常规/pre-pass/阴影等)来导入对应的代码及方法，
使用的时候需要注意调用对应标签宏的位置，正确的位置可以减少GPU的计算；<br>


## StandardVertexAttr.inc<br>
包含所有顶点属性，顶点着色器必须包含。<br>

## CommonStruct.inc<br>
封装了一些通用的顶点着色器和片段着色器的输入输出结构体，帮助简洁代码。比较蛋疼的是，
define XXX必须放在#include "CommonStruct.inc"之前，不然shader编译不会创建这些结构体。<br>

## Standard.inc<br>
包含引擎内部的所有标准uniform参数，以及对标签的定义，必须包含。<br>

## Math.inc<br>
通用的数学计算方法库，便于代码简洁。 <br>

## SkeletonAnimation.inc<br>
gpu动画相关的库，用于骨骼动画的蒙皮操作。 <br>

## Light.inc<br>
光照库，光照相关的uniform和参数都放在其中，使用到引擎内部的光照系统就必须包含。 <br>

