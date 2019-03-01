#include "stdafx.h"
#include "DownloadClient.h"

using namespace boost::asio;
using boost::asio::ip::tcp;

void mParseUrl(const char *mUrl, string &serverName, string &filepath, string &filename)
{
	string::size_type n;
	string url = mUrl;

	if (url.substr(0, 7) == "http://")
		url.erase(0, 7);

	if (url.substr(0, 8) == "https://")
		url.erase(0, 8);

	n = url.find('/');
	if (n != string::npos)
	{
		serverName = url.substr(0, n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n + 1);
	}

	else
	{
		serverName = url;
		filepath = "/";
		filename = "";
	}
}

void DownloadClient::DownloadData(const char *url, const char* fileName, bool toFile)
{
	string serverName; string filepath; string filename;
	mParseUrl(url, serverName, filepath, filename);
	string buffer;

	try
	{
		if (!toFile)
		{
			fileName = tmpnam(nullptr);
		}

		std::ofstream outFile(fileName, std::ofstream::binary);

		boost::asio::io_service io_service;

		// Get a list of endpoints corresponding to the server name.
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(serverName, "http");
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

		request_stream << "GET " << filepath << " HTTP/1.0\r\n";
		request_stream << "Host: " << serverName << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		// This buffer is used for reading and must be persisted
		//boost::beast::flat_buffer buffer;

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
		while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
		{
			outFile << &response;
		}
		outFile.close();
		
		if (Toolbelt::GetFileSize(fileName) < 2e+7)
		{
			if (!toFile && (buffer = string(Toolbelt::ReadFile(fileName, &length))).length())
			{
				dataBytes = new char[length];
				memcpy(dataBytes, buffer.c_str(), length);
			}
		}
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return;
	}
}

DownloadClient::DownloadClient(const char *url, const char* fileName, bool toFile)
{
	DownloadData(url, fileName, toFile);
}

DownloadClient::~DownloadClient()
{
}