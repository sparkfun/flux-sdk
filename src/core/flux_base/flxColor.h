
/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2026, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */
// flxCoreColor.h
//
// 2026 - kdb
// Useful in a variety of places. Might group with other
// defines in future.
//
//--------------------------------------------------------
// simple color typing
// For use in various parts of the framework / devices
// Could probably be moved to another header/block in the future
//
// The goals:
//      - define a type and then define some constant types/colors
//      - Easy to use syntax - make sense
//
// We could use a enum, but that limits value set to predefines, so
// use a namespace to segment the colors.
//
// Key point: Since this a namespace, other colors can be added in other files.
namespace flxColor
{
typedef uint32_t color;

// pre defiend colors
constexpr color Black = 0x000000;
constexpr color Blue = 0x0000FF;
constexpr color Green = 0x008000;
constexpr color Yellow = 0xFFFF00;
constexpr color Red = 0xFF0000;
constexpr color Gray = 0x808080;
constexpr color LightGray = 0x778899;
constexpr color Orange = 0xFFA500;
constexpr color White = 0xFFFFFF;
constexpr color Purple = 0x80008;

} // namespace flxColor
