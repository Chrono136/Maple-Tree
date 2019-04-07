namespace MapleSeed
{
	class MapleMain
	{
	public:
		static int g_init(signed int argc, char* argv[], std::string vers);

		static void simpleDecrypt(std::string path, std::string defaultdir);

	private:
		static po::options_description defineOptions();

		static void downloadcontent(std::string defaultdir, std::string id);

		static void create_decrypt_file(std::string _id);

		static void vmcount(po::variables_map vm, po::options_description desc, std::string defaultdir);
	};
}