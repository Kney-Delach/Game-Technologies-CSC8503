#pragma once
#include <string>
#include <iostream>
#include <fstream>

namespace NCL
{
	namespace CSC8503
	{		
		static std::string s_OutputFile = "output.txt";
		
		template <typename T>
		class FileManager
		{
		public:
			static void Loader(const std::string& filename, T& object);
			static void Writer(const std::string& filename, T& object);
		private:
			FileManager() = default;
			~FileManager() = default;
		};

		template<typename T>
		void FileManager<T>::Loader(const std::string& filename, T& object)
		{
			if (filename.empty())
				std::cout << "File argument must not be empty \n";
			std::string input;
			std::ifstream ifStream;
			ifStream.open(filename);			
			if (!ifStream)
			{
				const std::string msg = "Failed to open provided file [" + filename + "] \n";
				std::cout << msg;
				ifStream.close();
				return;
			}
			ifStream >> object; // loads the data from the file into the specified object.
			ifStream.close();
		}
		
		
		template<typename T>
		void FileManager<T>::Writer(const std::string& filename, T& object)
		{
			std::string input;
			std::ofstream fStream;
			fStream.open(filename);
			if(!fStream)
			{
				const std::string msg = "Failed to open provided file [" + filename + "] \n";
				std::cout << msg;
				fStream.close();
				return;
			}
			fStream << object;
			fStream.close();
		}
	}
}