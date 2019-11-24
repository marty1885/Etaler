#include <fstream>

#include "Serialize.hpp"

#include "Etaler/Core/Tensor.hpp"
#include "TypeHelpers.hpp"

using namespace et;

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace cereal
{

template <class Archive>
void save(Archive & archive , Shape const & s)
{
	std::vector<int64_t> vec(s.begin(), s.end());
	archive(vec);
}

template <class Archive>
void load(Archive & archive , Shape & s)
{
	std::vector<int64_t> vec;
	archive(vec);
	s = Shape(vec.begin(), vec.end());
}

template<class Archive>
void serialize(Archive & archive,
	half & m)
{
	static_assert(sizeof(m) == 2);
	#ifndef __aarch64__
	archive(m.storage_);
	#else
	uint16_t *p = (uint16_t*)&m;
	archive(*p);
	#endif
}



template <class Archive>
void save(Archive & archive, Tensor const & t)
{
	std::string dtype = [&t]() {
		if(t.dtype() == DType::Bool)
			return "uint8";
		if(t.dtype() == DType::Float)
			return "float";
		if(t.dtype() == DType::Int32)
			return "int32";
		if(t.dtype() == DType::Half)
			return "half";

		throw EtError("Cannot handle such dtype()");
	}();


	archive(make_nvp("shape", t.shape()));
	archive(make_nvp("dtype", dtype));
	if(t.dtype() == DType::Bool) {
		std::vector<uint8_t> arr = t.toHost<uint8_t>();
		archive(make_nvp("data", arr));
	}
	else if(t.dtype() == DType::Float) {
		std::vector<float> arr = t.toHost<float>();
		archive(make_nvp("data", arr));
	}
	else if(t.dtype() == DType::Int32) {
		std::vector<int32_t> arr = t.toHost<int32_t>();
		archive(make_nvp("data", arr));
	}
	else if(t.dtype() == DType::Half) {
		std::vector<half> arr = t.toHost<half>();
		archive(make_nvp("data", arr));
	}
}

template <class Archive>
void load(Archive & archive, Tensor & t)
{
	Shape s;
	archive(make_nvp("shape", s));

	std::string dtype;
	archive(make_nvp("dtype", dtype));

	if(dtype == "uint8") {
		std::vector<uint8_t> d(s.volume());
		archive(make_nvp("data", d));
		t = Tensor(s, d.data());
	}
	else if(dtype == "float") {
		std::vector<float> d(s.volume());
		archive(make_nvp("data", d));
		t = Tensor(s, d.data());
	}
	else if(dtype == "int32") {
		std::vector<int32_t> d(s.volume());
		archive(make_nvp("data", d));
		t = Tensor(s, d.data());
	}
	else if(dtype == "half") {
		std::vector<half> d(s.volume());
		archive(make_nvp("data", d));
		t = Tensor(s, d.data());
	}
}

template <class Archive>
void save(Archive & archive , std::vector<Tensor> const & v)
{
	archive(make_size_tag(static_cast<size_type>(v.size())));
	for(const auto& t : v)
		archive(t);
}

template <class Archive>
void load(Archive & archive , std::vector<Tensor> & v)
{
	size_type size;
	archive(make_size_tag(size));

	v.resize(size);
	for(auto& t : v)
		archive(t);
}

template <class Archive>
void save(Archive & archive ,StateDict const & item)
{
	std::vector<std::string> keys;
	std::vector<std::string> types;

	keys.reserve(item.size());
	types.reserve(item.size());

	for(const auto & [k, v] : item)
		keys.push_back(k);
	archive(make_nvp("keys", keys));

	for(const auto & [k, v] : item) {
		if(v.type() == typeid(std::string))
			types.push_back("string");
		else if(v.type() == typeid(Shape))
			types.push_back("Shape");
		else if(v.type() == typeid(int32_t))
			types.push_back("int32_t");
		else if(v.type() == typeid(float))
			types.push_back("float");
		else if(v.type() == typeid(bool))
			types.push_back("bool");
		else if(v.type() == typeid(Tensor))
			types.push_back("Tensor");
		else if(v.type() == typeid(StateDict))
			types.push_back("StateDict");
		else if(v.type() == typeid(std::vector<Tensor>))
			types.push_back("std::vector<Tensor>");
		else if(v.type() == typeid(std::vector<int>))
			types.push_back("std::vector<int>");
		else if(v.type() == typeid(std::vector<float>))
			types.push_back("std::vector<float>");
		else if(v.type() == typeid(std::vector<half>))
			types.push_back("std::vector<half>");
		else
			throw EtError("Cannot save type: " + demangle(v.type().name()) + ", key " + k);
	}
	archive(make_nvp("types", types));

	for(const auto & [k, v] : item) {
		if(v.type() == typeid(std::string))
			archive(std::any_cast<std::string>(v));
		else if(v.type() == typeid(Shape))
			archive(std::any_cast<Shape>(v));
		else if(v.type() == typeid(int32_t))
			archive(std::any_cast<int32_t>(v));
		else if(v.type() == typeid(float))
			archive(std::any_cast<float>(v));
		else if(v.type() == typeid(bool))
			archive(std::any_cast<bool>(v));
		else if(v.type() == typeid(Tensor))
			archive(std::any_cast<Tensor>(v));
		else if(v.type() == typeid(StateDict))
			archive(std::any_cast<StateDict>(v));
		else if(v.type() == typeid(std::vector<Tensor>))
			archive(std::any_cast<std::vector<Tensor>>(v));
		else if(v.type() == typeid(std::vector<int>))
			archive(std::any_cast<std::vector<int>>(v));
		else if(v.type() == typeid(std::vector<float>))
			archive(std::any_cast<std::vector<float>>(v));
		else if(v.type() == typeid(std::vector<half>))
			archive(std::any_cast<std::vector<half>>(v));
		else
			throw EtError("Cannot save type: " + demangle(typeid(decltype(v)).name()) + ", key " + k);

	}
}

template <typename T, class Archive>
void read_archive(Archive & archive, StateDict& dict, std::string key)
{
	T v;
	archive(v);
	dict[key] = v;
}

template <class Archive>
void load(Archive & archive ,StateDict & item)
{
	std::vector<std::string> keys;
	std::vector<std::string> types;

	archive(make_nvp("keys", keys));
	archive(make_nvp("types", types));

	et_assert(keys.size() == types.size());

	for(size_t i=0;i<keys.size();i++) {
		std::string key = keys[i];
		std::string type = types[i];
		if(type == "string")
			read_archive<std::string>(archive, item, key);
		else if(type == "Shape")
			read_archive<Shape>(archive, item, key);
		else if(type == "int32_t")
			read_archive<int32_t>(archive, item, key);
		else if(type == "float")
			read_archive<float>(archive, item, key);
		else if(type == "bool")
			read_archive<bool>(archive, item, key);
		else if(type == "Tensor")
			read_archive<Tensor>(archive, item, key);
		else if(type == "StateDict")
			read_archive<StateDict>(archive, item, key);
		else if(type == "std::vector<Tensor>")
			read_archive<std::vector<Tensor>>(archive, item, key);
		else if(type == "std::vector<int>")
			read_archive<std::vector<int>>(archive, item, key);
		else if(type == "std::vector<float>")
			read_archive<std::vector<float>>(archive, item, key);
		else if(type == "std::vector<half>")
			read_archive<std::vector<half>>(archive, item, key);
		else
			throw EtError("Cannot serealize type " + type);

	}
}

}

static std::string to_lower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

static std::string fileExtenstion(std::string path)
{
	size_t dot_pos = path.find_last_of('.');
	if(dot_pos == std::string::npos)
		return "";
	size_t slash_pos = path.find_last_of('/');
	if(slash_pos == std::string::npos)
		slash_pos = path.find_last_of('\\');
	if(slash_pos == std::string::npos || dot_pos > slash_pos)
		return to_lower(path.substr(dot_pos + 1));
	return "";
}

void et::save(const StateDict& dict, const std::string& path)
{
	std::ofstream out(path, std::ios::binary);

	std::string ext = fileExtenstion(path);
	if(ext == "json") {
		cereal::JSONOutputArchive ar(out);
		ar(dict);
	}
	else {
		cereal::PortableBinaryOutputArchive ar(out);
		ar(dict);
	}

}

StateDict et::load(const std::string& path)
{
	std::ifstream in(path, std::ios::binary);
	StateDict dict;

	std::string ext = fileExtenstion(path);
	if(ext == "json") {
		cereal::JSONInputArchive ar(in);
		ar(dict);
	}
	else {
		cereal::PortableBinaryInputArchive ar(in);
		ar(dict);
	}
	return dict;
}
