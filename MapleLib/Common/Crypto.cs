// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/06 3:28 PM
// 
// Project: MapleLib
// Filename: Crypto.cs
// Created By: Jared T

using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;

namespace MapleLib.Common
{
    public static class CDecrypt
    {
        public static int Extract(string tmdPath, string cetkPath, string outPath)
        {
            var bytes1 = Encoding.ASCII.GetBytes(tmdPath);
            var bytes2 = Encoding.ASCII.GetBytes(cetkPath);
            var bytes3 = Encoding.ASCII.GetBytes(outPath);

            var result = 0;
            /*
            unsafe
            {
                var wrapper = new CDecryptWrapper.Cpp.CLI.Decrypt();

                fixed (byte* p1 = bytes1)
                {
                    fixed (byte* p2 = bytes2)
                    {
                        fixed (byte* p3 = bytes3)
                        {
                            var sp1 = (sbyte*)p1;
                            var sp2 = (sbyte*)p2;
                            var sp3 = (sbyte*)p3;

                            result = wrapper.decrypt(3, sp1, sp2, sp3);
                        }
                    }
                }
            }
            */
            return result;
        }
    }

    public static class Crypto
    {
        private static readonly string PasswordHash = "mapleseed584723";
        private static readonly string SaltKey = "FVS4th65h#@W$QyyW@";
        private static readonly string VIKey = "@1B2c3D42fd7g7H8";

        public static string Encrypt(string plainText)
        {
            var plainTextBytes = Encoding.UTF8.GetBytes(plainText);

            var keyBytes = new Rfc2898DeriveBytes(PasswordHash, Encoding.ASCII.GetBytes(SaltKey)).GetBytes(256 / 8);
            var symmetricKey = new RijndaelManaged {Mode = CipherMode.CBC, Padding = PaddingMode.Zeros};
            var encryptor = symmetricKey.CreateEncryptor(keyBytes, Encoding.ASCII.GetBytes(VIKey));

            byte[] cipherTextBytes;

            using (var memoryStream = new MemoryStream())
            {
                using (var cryptoStream = new CryptoStream(memoryStream, encryptor, CryptoStreamMode.Write))
                {
                    cryptoStream.Write(plainTextBytes, 0, plainTextBytes.Length);
                    cryptoStream.FlushFinalBlock();
                    cipherTextBytes = memoryStream.ToArray();
                    cryptoStream.Close();
                }
                memoryStream.Close();
            }
            return Convert.ToBase64String(cipherTextBytes);
        }

        public static string Decrypt(string encryptedText)
        {
            var cipherTextBytes = Convert.FromBase64String(encryptedText);
            var keyBytes = new Rfc2898DeriveBytes(PasswordHash, Encoding.ASCII.GetBytes(SaltKey)).GetBytes(256 / 8);
            var symmetricKey = new RijndaelManaged {Mode = CipherMode.CBC, Padding = PaddingMode.None};

            var decryptor = symmetricKey.CreateDecryptor(keyBytes, Encoding.ASCII.GetBytes(VIKey));
            var memoryStream = new MemoryStream(cipherTextBytes);
            var cryptoStream = new CryptoStream(memoryStream, decryptor, CryptoStreamMode.Read);
            var plainTextBytes = new byte[cipherTextBytes.Length];

            var decryptedByteCount = cryptoStream.Read(plainTextBytes, 0, plainTextBytes.Length);
            memoryStream.Close();
            cryptoStream.Close();
            return Encoding.UTF8.GetString(plainTextBytes, 0, decryptedByteCount).TrimEnd("\0".ToCharArray());
        }
    }
}