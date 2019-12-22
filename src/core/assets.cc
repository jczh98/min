// MIT License
//
// Copyright (c) 2018-2019, neverfelly <neverfelly@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <min-ray/assets.h>

namespace min::refl {

Object* Assets::LoadAsset(const std::string& name, const std::string& impl_key, const json& json) {
	const auto it = asset_index_map_.find(name);
	const auto found = it == asset_index_map_.end();
	if (found) {

	}
	auto p = CreateWithoutInitialize<Object>(impl_key, MakeLoc(loc(), name));
	if (!p) {
		// Handle create object error
		return nullptr;
	}
	Object *asset;
	if (found) {
		auto old = std::move(assets_[it->second]);
		assets_[it->second] = std::move(p);
		asset = assets_[it->second].get();

		asset->Initialize(json);

		const Object::Visitor visitor = [&](Object*& p, bool weak) {
			if (!p) {
				return;
			}
			if (asset == p) {
				return;
			}
			if (!weak) {
				p->ForeachUnderlying(visitor);
			} else {
				UpdateWeakRef(p);
			}
		};
		get<Object>("$")->ForeachUnderlying(visitor);
	}	else {
		asset_index_map_[name] = int(assets_.size());
		assets_.emplace_back(std::move(p));
		asset = assets_.back().get();

		asset->Initialize(json);
	}
	return asset;
}
}
