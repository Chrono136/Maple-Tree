// Created: 2017/11/19 8:35 PM
// Updated: 2017/11/19 8:48 PM
// 
// Project: Maple.Error
// Filename: DateTimeWithZone.cs
// Created By: Jared T

using System;

namespace Maple.Error
{
    public static class DateTimeWithZone
    {
        static DateTimeWithZone()
        {
            var dateTimeUnspec = DateTime.SpecifyKind(DateTime.Now, DateTimeKind.Unspecified);
            UniversalTime = TimeZoneInfo.ConvertTimeToUtc(dateTimeUnspec, TimeZoneInfo.Utc);
            TimeZone = TimeZoneInfo.Utc;
        }

        public static DateTime UniversalTime { get; }

        private static TimeZoneInfo TimeZone { get; }

        public static DateTime LocalTime => TimeZoneInfo.ConvertTime(UniversalTime, TimeZone);
    }
}