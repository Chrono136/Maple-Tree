// Created: 2017/10/02 9:55 AM
// Updated: 2017/10/02 9:55 AM
// 
// Project: MapleLib
// Filename: TitleConverter.cs
// Created By: Jared T

using System;
using MapleLib.Structs;
using Newtonsoft.Json;

namespace MapleLib.Common.JsonConverter
{
    public class VersionConverter : Newtonsoft.Json.JsonConverter
    {
        public override bool CanConvert(Type objectType)
        {
            return objectType == typeof(Versions);
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            var str = serializer.Deserialize<string>(reader);
            Versions versions = str;
            return versions;
        }

        public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
        {
            throw new NotImplementedException();
        }
    }
}