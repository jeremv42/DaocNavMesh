#include "niflib.hpp"

#include <set>

#include <gen/Header.h>
#include <obj/NiObject.h>
#include <obj/NiNode.h>
#include <obj/NiTriShape.h>
#include <obj/NiTriShapeData.h>
#include <obj/NiTriStrips.h>
#include <obj/NiTriStripsData.h>

using namespace Niflib;

NifInfo ReadHeaderInfo(std::istream &in)
{
	Header nif_header;
	NifInfo info;
	info = nif_header.Read(in);
	return info;
}

NiAVObjectRef _FindNode(NiNodeRef node, std::set<std::string> const &names)
{
	if (!node)
		return nullptr;

	for (auto childAV : node->GetChildren())
	{
		auto name = childAV->GetName();
		std::transform(name.begin(), name.end(), name.begin(), [](char c)
					   { return std::tolower(c); });
		if (names.contains(name))
			return childAV;

		auto res = _FindNode(DynamicCast<NiNode>(childAV), names);
		if (res)
			return res;
	}
	return nullptr;
}

template<typename T>
std::vector<NiTriShapeRef> _FindTriShapes(Ref<T> &obj)
{
	auto tri = DynamicCast<NiTriShape>(obj);
	if (tri)
		return {tri};
	auto node = DynamicCast<NiNode>(obj);
	if (!node)
		return {};
	std::vector<NiTriShapeRef> res;
	for (auto child : node->GetChildren())
	{
		auto childRes = _FindTriShapes(child);
		if (childRes.empty())
			continue;
		for (auto &ref : childRes)
			res.emplace_back(ref);
	}
	return res;
}
template<typename T>
std::vector<NiTriStripsRef> _FindTriStrips(Ref<T> &obj)
{
	auto tri = DynamicCast<NiTriStrips>(obj);
	if (tri)
		return {tri};
	auto node = DynamicCast<NiNode>(obj);
	if (!node)
		return {};
	std::vector<NiTriStripsRef> res;
	for (auto child : node->GetChildren())
	{
		auto childRes = _FindTriStrips(child);
		if (childRes.empty())
			continue;
		for (auto &ref : childRes)
			res.emplace_back(ref);
	}
	return res;
}

bool Niflib::TryExtractMesh(NiObject *obj, glm::mat4 const &nifWorld, std::vector<glm::vec3> &vertices, std::vector<uint16_t> &indices)
{
	auto node = dynamic_cast<NiNode *>(obj);
	if (!node)
		return false;

	auto root = _FindNode(node, {"collisionswitch"});
	if (root)
		root = _FindNode(DynamicCast<NiNode>(root), {"collide", "collidee", "collision"});
	if (!root)
		root = _FindNode(node, {"collide", "collidee", "collision"});
	if (!root)
		root = _FindNode(node, {"visible"});
	if (!root)
		return false;

	auto triShapes = _FindTriShapes(root);
	for (auto &shape: triShapes)
	{
		auto index = uint16_t(vertices.size());
		auto data = DynamicCast<NiTriShapeData>(shape->GetData());
		auto world = nifWorld * ToMat4(shape->GetWorldTransform());
		for (auto &v : shape->GetData()->GetVertices())
			vertices.push_back(world * ToVec4(v));
		for (auto tri : data->GetTriangles())
		{
			indices.push_back(uint16_t(tri.v1) + index);
			indices.push_back(uint16_t(tri.v2) + index);
			indices.push_back(uint16_t(tri.v3) + index);
		}
	}
	auto triStrips = _FindTriStrips(root);
	for (auto &strip: triStrips)
	{
		auto index = uint16_t(vertices.size());
		auto data = DynamicCast<NiTriStripsData>(strip->GetData());
		auto world = nifWorld * ToMat4(strip->GetWorldTransform());
		for (auto &v : data->GetVertices())
			vertices.push_back(world * ToVec4(v));
		for (auto tri : data->GetTriangles())
		{
			indices.push_back(uint16_t(tri.v1) + index);
			indices.push_back(uint16_t(tri.v2) + index);
			indices.push_back(uint16_t(tri.v3) + index);
		}
	}

	return true;
}
