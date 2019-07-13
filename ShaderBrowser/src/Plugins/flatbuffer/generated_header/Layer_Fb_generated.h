// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_LAYERFB_LAYER_H_
#define FLATBUFFERS_GENERATED_LAYERFB_LAYER_H_

#include "flatbuffers/flatbuffers.h"

namespace Layer {

struct Point_Fb;

struct Node_Fb;

struct Layer_Fb;

struct Point_Fb FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X) &&
           VerifyField<float>(verifier, VT_Y) &&
           verifier.EndTable();
  }
};

struct Point_FbBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(Point_Fb::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Point_Fb::VT_Y, y, 0.0f);
  }
  explicit Point_FbBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  Point_FbBuilder &operator=(const Point_FbBuilder &);
  flatbuffers::Offset<Point_Fb> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Point_Fb>(end);
    return o;
  }
};

inline flatbuffers::Offset<Point_Fb> CreatePoint_Fb(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f) {
  Point_FbBuilder builder_(_fbb);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

struct Node_Fb FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_POSITION = 6
  };
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  const Layer::Point_Fb *position() const {
    return GetPointer<const Layer::Point_Fb *>(VT_POSITION);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_POSITION) &&
           verifier.VerifyTable(position()) &&
           verifier.EndTable();
  }
};

struct Node_FbBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(Node_Fb::VT_NAME, name);
  }
  void add_position(flatbuffers::Offset<Layer::Point_Fb> position) {
    fbb_.AddOffset(Node_Fb::VT_POSITION, position);
  }
  explicit Node_FbBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  Node_FbBuilder &operator=(const Node_FbBuilder &);
  flatbuffers::Offset<Node_Fb> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Node_Fb>(end);
    return o;
  }
};

inline flatbuffers::Offset<Node_Fb> CreateNode_Fb(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    flatbuffers::Offset<Layer::Point_Fb> position = 0) {
  Node_FbBuilder builder_(_fbb);
  builder_.add_position(position);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<Node_Fb> CreateNode_FbDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    flatbuffers::Offset<Layer::Point_Fb> position = 0) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return Layer::CreateNode_Fb(
      _fbb,
      name__,
      position);
}

struct Layer_Fb FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SUPER = 4,
    VT_FRIEND_ = 6,
    VT_CHILDREN = 8,
    VT_TRANSFORM = 10
  };
  const Layer::Node_Fb *super() const {
    return GetPointer<const Layer::Node_Fb *>(VT_SUPER);
  }
  const Layer::Node_Fb *friend_() const {
    return GetPointer<const Layer::Node_Fb *>(VT_FRIEND_);
  }
  const flatbuffers::Vector<flatbuffers::Offset<Layer::Node_Fb>> *children() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Layer::Node_Fb>> *>(VT_CHILDREN);
  }
  const flatbuffers::Vector<flatbuffers::Offset<Layer::Point_Fb>> *transform() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Layer::Point_Fb>> *>(VT_TRANSFORM);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_SUPER) &&
           verifier.VerifyTable(super()) &&
           VerifyOffset(verifier, VT_FRIEND_) &&
           verifier.VerifyTable(friend_()) &&
           VerifyOffset(verifier, VT_CHILDREN) &&
           verifier.VerifyVector(children()) &&
           verifier.VerifyVectorOfTables(children()) &&
           VerifyOffset(verifier, VT_TRANSFORM) &&
           verifier.VerifyVector(transform()) &&
           verifier.VerifyVectorOfTables(transform()) &&
           verifier.EndTable();
  }
};

struct Layer_FbBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_super(flatbuffers::Offset<Layer::Node_Fb> super) {
    fbb_.AddOffset(Layer_Fb::VT_SUPER, super);
  }
  void add_friend_(flatbuffers::Offset<Layer::Node_Fb> friend_) {
    fbb_.AddOffset(Layer_Fb::VT_FRIEND_, friend_);
  }
  void add_children(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Layer::Node_Fb>>> children) {
    fbb_.AddOffset(Layer_Fb::VT_CHILDREN, children);
  }
  void add_transform(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Layer::Point_Fb>>> transform) {
    fbb_.AddOffset(Layer_Fb::VT_TRANSFORM, transform);
  }
  explicit Layer_FbBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  Layer_FbBuilder &operator=(const Layer_FbBuilder &);
  flatbuffers::Offset<Layer_Fb> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Layer_Fb>(end);
    return o;
  }
};

inline flatbuffers::Offset<Layer_Fb> CreateLayer_Fb(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<Layer::Node_Fb> super = 0,
    flatbuffers::Offset<Layer::Node_Fb> friend_ = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Layer::Node_Fb>>> children = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Layer::Point_Fb>>> transform = 0) {
  Layer_FbBuilder builder_(_fbb);
  builder_.add_transform(transform);
  builder_.add_children(children);
  builder_.add_friend_(friend_);
  builder_.add_super(super);
  return builder_.Finish();
}

inline flatbuffers::Offset<Layer_Fb> CreateLayer_FbDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<Layer::Node_Fb> super = 0,
    flatbuffers::Offset<Layer::Node_Fb> friend_ = 0,
    const std::vector<flatbuffers::Offset<Layer::Node_Fb>> *children = nullptr,
    const std::vector<flatbuffers::Offset<Layer::Point_Fb>> *transform = nullptr) {
  auto children__ = children ? _fbb.CreateVector<flatbuffers::Offset<Layer::Node_Fb>>(*children) : 0;
  auto transform__ = transform ? _fbb.CreateVector<flatbuffers::Offset<Layer::Point_Fb>>(*transform) : 0;
  return Layer::CreateLayer_Fb(
      _fbb,
      super,
      friend_,
      children__,
      transform__);
}

inline const Layer::Layer_Fb *GetLayer_Fb(const void *buf) {
  return flatbuffers::GetRoot<Layer::Layer_Fb>(buf);
}

inline const Layer::Layer_Fb *GetSizePrefixedLayer_Fb(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<Layer::Layer_Fb>(buf);
}

inline bool VerifyLayer_FbBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<Layer::Layer_Fb>(nullptr);
}

inline bool VerifySizePrefixedLayer_FbBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<Layer::Layer_Fb>(nullptr);
}

inline void FinishLayer_FbBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Layer::Layer_Fb> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedLayer_FbBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Layer::Layer_Fb> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace Layer

#endif  // FLATBUFFERS_GENERATED_LAYERFB_LAYER_H_