#include "BinaryFileUtils.h"
#ifdef _WIN32
    // WIN32
    #include <corecrt_io.h>
    #pragma warning(disable:4996)
#else
    // macOS
    #include <unistd.h>
#endif
#include "Utils.h"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/util.h>
#include <Layer_Fb_generated.h>
#include <MyFlatStruct_generated.h>
#include <iostream>

namespace common
{
	BinaryFileUtils::BinaryFileUtils()
	{
	}

	BinaryFileUtils::~BinaryFileUtils()
	{
	}

	void BinaryFileUtils::serialize()
	{
		flatbuffers::FlatBufferBuilder build_data;

		// super
		auto position_fb = Layer::CreatePoint_Fb(build_data, 1, 1);
		auto super_fb = Layer::CreateNode_Fb(build_data, build_data.CreateString("father"), position_fb);

		// friend
		auto friend_fb = Layer::CreateNode_Fb(build_data, build_data.CreateString("tom"), Layer::CreatePoint_Fb(build_data, 2, 2));

		// children
		std::vector<flatbuffers::Offset<Layer::Node_Fb>> nodeVector;
		auto child1 = Layer::CreateNode_Fb(build_data, build_data.CreateString("son"), Layer::CreatePoint_Fb(build_data, 3, 3));
		auto child2 = Layer::CreateNode_Fb(build_data, build_data.CreateString("daughter"), Layer::CreatePoint_Fb(build_data, 4, 4));
		nodeVector.push_back(child1);
		nodeVector.push_back(child2);

		auto children_fb = build_data.CreateVector(nodeVector);

		// layer
		auto layer_fb = Layer::CreateLayer_Fb(build_data, super_fb, friend_fb, children_fb);
		build_data.Finish(layer_fb);

		flatbuffers::SaveFile("layer.bytes", reinterpret_cast<char*>(build_data.GetBufferPointer()), build_data.GetSize(), true);
	}

	void BinaryFileUtils::deserialize()
	{
		// load binary file
		std::string binaryfile;
		bool ok = flatbuffers::LoadFile("layer.bytes", false, &binaryfile);
		if (!ok) {
			return;
		}


		flatbuffers::FlatBufferBuilder builder_out;
		builder_out.PushBytes(reinterpret_cast<unsigned char*>(const_cast<char*>(binaryfile.c_str())), binaryfile.size());
		cout << "deserialize size:" << builder_out.GetSize() << endl;

		// verify
		flatbuffers::Verifier layer_verify(builder_out.GetCurrentBufferPointer(), builder_out.GetSize());
		bool verify_flag = Layer::VerifyLayer_FbBuffer(layer_verify);
		if (!verify_flag) {
			return;
		}

		// read
		auto layer_fb = Layer::GetLayer_Fb(builder_out.GetCurrentBufferPointer());

		cout << "father name:" + layer_fb->super()->name()->str() << endl;
		cout << "father position:" << layer_fb->super()->position()->x() << ", " << layer_fb->super()->position()->y() << endl;
		cout << "friend name:" + layer_fb->friend_()->name()->str() << endl;
		cout << "friend position:" << layer_fb->friend_()->position()->x() << ", " << layer_fb->super()->position()->y() << endl;

		for (int i = 0; i < layer_fb->children()->Length(); i++) {
			string name = layer_fb->children()->Get(i)->name()->str();
			int x = layer_fb->children()->Get(i)->position()->x();
			int y = layer_fb->children()->Get(i)->position()->y();
			cout << "child " << i << ":" + name << x << " , " << y << endl;
		}
	}


}
