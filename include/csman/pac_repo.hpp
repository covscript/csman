#pragma once
/**
 * CovScript Manager: Package Repository
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/global.hpp>
#include <iostream>
#include <fstream>

namespace csman {
	class pac_repo {
	private:
		context *cxt;
	public:/*公开类*/
		class pac_data {
		public:
			std::string available;
			set_t<std::string> ver;
		};

		map_t<std::string, pac_data> local_pac;
	private:/*私有方法*/
		void read_file(std::ifstream &ifs)
		{
			std::vector<std::string> args;
			while (!readline(ifs, args)) {
				local_pac[args[0]] = pac_data();

				auto &x = local_pac[args[0]];
				for (int i = 1; i < args.size(); i++)
					x.ver.insert(args[i]);
				x.available = args[1];
			}
		}

		void write_file(std::ofstream &ofs)
		{
			for (auto &x : local_pac) {
				ofs << x.first << " ";
				ofs << x.second.available << " ";
				for (auto &y : x.second.ver)
					if (y != x.second.available)
						ofs << y << " ";
				ofs << std::endl;
			}
		}

	public:/*公开接口*/
		pac_repo() = default;

		pac_repo(context *cxt) : cxt(cxt)
		{
			std::string path;
			if(cxt->vars.count("pac_repo_path") != 0) {
				path = cxt->vars["pac_repo_path"];
				std::ifstream ifs(path);
				if (!ifs.is_open())
					throw std::runtime_error("opening \"pac_repo\" failed.");
				read_file(ifs);
				ifs.close();
			}
			else
				throw std::runtime_error("csman has a bug which can not get variable named \"pac_repo_path\" in context.");
			return;
		}

		~pac_repo()
		{
			std::string path;
			std::cout << std::endl;
			std::cout<<cxt->vars.count("pac_repo_path");
			if(cxt->vars.count("pac_repo_path") != 0) {
				path = cxt->vars["pac_repo_path"];
				std::ofstream ofs(path);
				write_file(ofs);
				ofs.close();
			}
			else
				std::cout<<"Warning: your file for recording pac_repo updating failed, it may cause extremely problems while next time. Please try to repair it by using \"csman repair\"";
			return;
		}

		void update_install(const std::string &name, const std::string &ver, bool is_available)
		{
			if (local_pac.count(name) == 0)
				local_pac[name] = pac_data();
			if (is_available)
				local_pac[name].available = ver;
			local_pac[name].ver.insert(ver);

			return;
		}

		void update_uninstall(const std::string &name, const std::string &ver)
		{
			auto it = local_pac[name].ver.find(ver);
			if (local_pac.count(name) == 0 || it == local_pac[name].ver.end())
				throw std::invalid_argument("package \"" + name + " " + ver + "\" is not existed.");
			if (local_pac[name].available == ver)
				local_pac[name].available.clear();
			local_pac[name].ver.erase(ver);
			return;
		}

		void update_checkout()
		{

		}

		inline set_t<std::string> query_contains_ver(const std::string &name)
		{
			set_t<std::string> query_reasult;
			if (local_pac.count(name) != 0) {
				for (auto ver : local_pac[name].ver)
					query_reasult.insert(ver);
			}
			return query_reasult;
		}

		inline std::string query_using_ver(const std::string &name)
		{
			return local_pac.count(name) == 0 ? "" : /*likely*/ local_pac[name].available;
		}

		inline std::string get_current_runtime_ver()
		{
			return cxt->runtime_ver;
		}
	};
}