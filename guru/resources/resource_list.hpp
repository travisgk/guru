/**
 * resource_list.hpp
 * ---
 * this file defines the <ResourceList> template class, which will maintain a list
 * of a particular kind of Resource loaded into Guru. it will also maintain how
 * many sources are making use of each Resource, and will delete the Resource
 * from memory only when there are no more sources are making use of it.
 * 
 */

#pragma once
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>
#include "../system/settings.hpp"
#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
#include <iostream>
#endif

namespace gu {
namespace res {
template <typename T>
class ResourceList {
private:
	const std::string CLASS_TYPE;
	std::hash<std::string> _hasher;
	std::unordered_map<size_t, std::shared_ptr<T>> _hash_to_pointer;
	size_t _last_hash_num = 0;

public:
	ResourceList(const std::string &class_type) : CLASS_TYPE(class_type) {}

	~ResourceList() {
		deallocate();
	}

	// clears every contained item from memory.
	void deallocate() {
		for (const auto &pair : _hash_to_pointer)
			_delete_resource(pair.second);
		_hash_to_pointer.clear();
	}

	std::shared_ptr<T> find_existing(
		const std::filesystem::path &path
	) const {
		size_t hash_num = _hasher(path.string());
		auto iter = _hash_to_pointer.find(hash_num);

		#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
		if (iter != _hash_to_pointer.end()) {
			std::cout 
				<< path.string() << "(hash_num: " << std::to_string(hash_num)
				<< ") was found." << std::endl;
			return iter->second;
		}
		std::cout 
			<< path.string() << "(hash_num: " << std::to_string(hash_num) 
			<< " was not found in this ResourceList of length " 
			<< std::to_string(_hash_to_pointer.size()) << std::endl;
		return nullptr;
		#else
		return iter != _hash_to_pointer.end() ? iter->second : nullptr;
		#endif
	}

	void add_entry(const std::shared_ptr<T> &res) {
		size_t hash_num = _hasher(res->get_path().string());
		#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
		std::cout 
			<< CLASS_TYPE << ": adding entry. _hash_to_pointer.size() was: " 
			<< std::to_string(_hash_to_pointer.size()) 
			<< "\thash_num for " << res->path() << " is: "
			<< std::to_string(hash_num) << std::endl;
		#endif

		auto iter = _hash_to_pointer.find(hash_num);
		if (iter != _hash_to_pointer.end()) {
			#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
			std::cout 
				<< "hash_num for " << res->path() 
				<< " is found!" << std::endl;
			#endif
			return;
		}
		_last_hash_num = hash_num;
		_hash_to_pointer[hash_num] = res;

		#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
		std::cout
			<< CLASS_TYPE
			<< ": entry added. _hash_to_pointer.size() is: "
			<< std::to_string(_hash_to_pointer.size()) << std::endl;
		#endif
	}

	void create_entry(const T &res) {
		size_t hash_num = _hasher(res.get_path().string());
		#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
		std::cout
			<< CLASS_TYPE << ": creating entry. _hash_to_pointer.size() was: "
			<< std::to_string(_hash_to_pointer.size())
			<< "\thash_num for " << res.path() << " is: "
			<< std::to_string(hash_num) << std::endl;
		#endif
		auto iter = _hash_to_pointer.find(hash_num);
		if (iter != _hash_to_pointer.end()) {
			#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
			std::cout
				<< "hash_num for " << res.path()
				<< " is found!" << std::endl;
			#endif
			return;
		}
		_last_hash_num = hash_num;
		_hash_to_pointer[hash_num] = std::make_shared<T>(res);

		#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
		std::cout
			<< CLASS_TYPE << ": entry created. _hash_to_pointer.size() is: "
			<< std::to_string(_hash_to_pointer.size()) << std::endl;
		#endif
	}

	std::shared_ptr<T> get_last_created() const {
		if (not _last_hash_num)
			return nullptr;
		auto iter = _hash_to_pointer.find(_last_hash_num);
		return iter->second;
	}

	// removes the ResourceList's hash entry mapped to the shared pointer,
	// only if it's no longer being externally used,
	// and finally calls the virtual method _delete_resource(),
	// which is designed to be overridden for deallocating
	// child class-specific datatypes.
	void delete_entry(const std::filesystem::path &path) {
		size_t hash_num = _hasher(path.string());
		#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
		std::cout
			<< "deleting entry. _hash_to_pointer.size() was: "
			<< std::to_string(_hash_to_pointer.size())
			<< "\thash_num for " << path << " is: "
			<< std::to_string(hash_num) << std::endl;
		#endif

		auto iter = _hash_to_pointer.find(hash_num);
		if (iter == _hash_to_pointer.end()) {
			#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
			std::cout
				<< "hash_num was not found in _hash_to_pointer, "
				<< "so nothing was deleted." << std::endl;
			#endif
			return;
		} else if (iter->second.use_count() > 1) {
			#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
			std::cout
				<< CLASS_TYPE << ": shared pointer is still in use with "
				<< std::to_string(iter->second.use_count()) << " instances, "
				<< "so nothing was deleted." << std::endl;
			#endif
			return;
		}

		#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
		std::cout 
			<< CLASS_TYPE << ": _hash_to_pointer.size() was: "
			<< std::to_string(_hash_to_pointer.size())
			<< "\thash_num for " << path << " is: "
			<< std::to_string(hash_num) << std::endl;
		#endif
		_delete_resource(iter->second);
		_hash_to_pointer.erase(hash_num);

		#if defined(GURU_PRINT_RESOURCE_DEBUG_MESSAGES)
		std::cout
			<< CLASS_TYPE << ": _hash_to_pointer.size() is: "
			<< std::to_string(_hash_to_pointer.size()) << std::endl;
		#endif
	}

protected:
	// this method exists to be overridden by child classes
	// in order to handle specialized deletion processes of an entry.
	inline virtual void _delete_resource(const std::shared_ptr<T> &res_ptr) {}
};
} // namespace res
} // namespace gu