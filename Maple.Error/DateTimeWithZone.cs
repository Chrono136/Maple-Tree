// Created: 2017/11/19 8:35 PM
// Updated: 2017/11/19 8:48 PM
// 
// Project: Maple.Error
// Filename: DateTimeWithZone.cs
// Created By: Jared T

using System;

namespace Maple.Error
{
    public struct DateTimeWithZone
    {
        public DateTimeWithZone(DateTime dateTime, TimeZoneInfo timeZone)
        {
            var dateTimeUnspec = DateTime.SpecifyKind(dateTime, DateTimeKind.Unspecified);
            UniversalTime = TimeZoneInfo.ConvertTimeToUtc(dateTimeUnspec, timeZone);
            TimeZone = timeZone;
        }

        public DateTime UniversalTime { get; }

        public TimeZoneInfo TimeZone { get; }

        public DateTime LocalTime => TimeZoneInfo.ConvertTime(UniversalTime, TimeZone);
    }
}