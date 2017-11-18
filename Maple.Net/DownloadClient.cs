// Created: 2017/11/18 2:20 PM
// Updated: 2017/11/18 2:44 PM
// 
// Project: Maple.Net
// Filename: DownloadClient.cs
// Created By: Jared T

using System;
using System.Diagnostics.Contracts;
using System.IO;
using System.Net;

namespace Maple.Net
{
    public class DownloadClient
    {
        public byte[] DownloadData(string url, string fileName)
        {
            Contract.Requires(!string.IsNullOrEmpty(url), "DownloadData 'URL' can not be null or empty");
            Contract.Ensures(Contract.Result<byte[]>() != null);

            var tempFile = Path.Combine(Path.GetTempPath(), Path.GetTempFileName());
            
            var request = (HttpWebRequest) WebRequest.Create(url);
            request.Timeout = 5000;

            try
            {
                using (WebResponse response = (HttpWebResponse)request.GetResponse())
                {
                    using (var stream = new FileStream(fileName, FileMode.Create, FileAccess.Write))
                    {
                        var bytes = ReadFully(response.GetResponseStream());

                        stream.Write(bytes, 0, bytes.Length);
                    }
                }

                if (!File.Exists(fileName))
                    File.Move(tempFile, fileName);
            }
            catch (WebException)
            {
                Console.WriteLine("Error Occured");
            }

            return new byte[] { };
        }

        private static byte[] ReadFully(Stream input)
        {
            var buffer = new byte[16 * 1024];
            using (var ms = new MemoryStream())
            {
                int read;
                while ((read = input.Read(buffer, 0, buffer.Length)) > 0)
                    ms.Write(buffer, 0, read);

                return ms.ToArray();
            }
        }
    }
}