#include "stdafx.h"
#include "DownloadClient.h"

using namespace boost::asio;
using boost::asio::ip::tcp;

void DownloadClient::DownloadFile(const char *url, const char*fn)
{
	string psn;string pfp;string pfn;
	ParseUrl(url, psn, pfp, pfn);

	try
	{
		std::ofstream os(fn, std::ofstream::binary);
		boost::asio::io_service io_service;

		// Get a list of endpoints corresponding to the server name.
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(psn, "http");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;

		// Try each endpoint until we successfully establish a connection.
		tcp::socket socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			socket.close();
			socket.connect(*endpoint_iterator++, error);
		}

		boost::asio::streambuf request;
		std::ostream request_stream(&request);

		request_stream << "GET " << pfp << " HTTP/1.0\r\n";
		request_stream << "Host: " << psn << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		// Send the request.
		boost::asio::write(socket, request);

		// Read the response status line.
		boost::asio::streambuf response;
		boost::asio::read_until(socket, response, "\r\n");

		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);

		// Read the response headers, which are terminated by a blank line.
		boost::asio::read_until(socket, response, "\r\n\r\n");

		// Process the response headers.
		std::string header;
		while (std::getline(response_stream, header) && header != "\r") {}

		// Write whatever content we already have to output.
		if (response.size() > 0)
			os << &response;

		// Read until EOF, writing data to output as we go.
		while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
			os << &response;
		os.close();

		int bLen = 1024 * 10;
		buf = new char[bLen];
		std::ifstream is(fn, std::ifstream::binary);
		is.seekg(0, is.end);
		len = is.tellg();
		is.seekg(0, is.beg);
		while (is)
			is.read(buf, bLen);
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return;
	}
}

void DownloadClient::DownloadData(const char *url, const char* fileName, unsigned long rsize, bool toFile, bool resume)
{
	string pserverName; string pfilepath; string pfilename;
	ParseUrl(url, pserverName, pfilepath, pfilename);

	unsigned long csize;
	std::ofstream outFile;

	try
	{
		if (!toFile)
		{
			fileName = tmpnam(nullptr);
		}
		
		if ((csize = CommonTools::GetFileSize(fileName)) > rsize)
			outFile = std::ofstream(fileName, std::ofstream::binary);
		else
			outFile = std::ofstream(fileName, std::ofstream::binary | std::ios_base::app);

		boost::asio::io_service io_service;

		// Get a list of endpoints corresponding to the server name.
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(pserverName, "http");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;

		// Try each endpoint until we successfully establish a connection.
		tcp::socket socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			socket.close();
			socket.connect(*endpoint_iterator++, error);
		}

		boost::asio::streambuf request;
		std::ostream request_stream(&request);

		request_stream << "GET " << pfilepath << " HTTP/1.0\r\n";
		request_stream << "Host: " << pserverName << "\r\n";
		request_stream << "Accept: */*\r\n";
		if (resume) //setup download resume
		{
			outFile.seekp(0, ios::end);
			request_stream << "Range: bytes=" << csize << "-" << rsize << "\r\n";
		}
		request_stream << "Connection: close\r\n\r\n";

		// Send the request.
		boost::asio::write(socket, request);

		// Read the response status line.
		boost::asio::streambuf response;
		boost::asio::read_until(socket, response, "\r\n");

		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);

		// Read the response headers, which are terminated by a blank line.
		boost::asio::read_until(socket, response, "\r\n\r\n");

		// Process the response headers.
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
		{
		}

		// Write whatever content we already have to output.
		if (response.size() > 0)
		{
			outFile << &response;
		}

		// Read until EOF, writing data to output as we go.
		if (resume)
		{
			boost::progress_display progress(rsize);
			while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
			{
				outFile << &response;
				progress += (unsigned long)outFile.tellp() - progress.count();
			}
		}
		else
		{
			while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
			{
				outFile << &response;
			}
		}
		outFile.close();
		
		if (GetFileSize(fileName) < 2e+7)
		{
			string buffer;
			if (!toFile && (buffer = string(ReadFile(fileName, &len))).length())
			{
				buf = new char[len];
				memcpy(buf, buffer.c_str(), len);
			}
		}
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return;
	}
}

DownloadClient::DownloadClient(const char *url, const char* fileName, unsigned long filesize, bool toFile, bool resume)
{
	DownloadData(url, fileName, filesize, toFile, resume);
}

DownloadClient::~DownloadClient()
{
}