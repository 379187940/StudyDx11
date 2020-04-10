/*
 *  Copyright 2019-2020 Diligent Graphics LLC
 *  Copyright 2015-2019 Egor Yusov
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence), 
 *  contract, or otherwise, unless required by applicable law (such as deliberate 
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental, 
 *  or consequential damages of any character arising as a result of this License or 
 *  out of the use or inability to use the software (including but not limited to damages 
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and 
 *  all other commercial damages or losses), even if such Contributor has been advised 
 *  of the possibility of such damages.
 */

#pragma once

//#include "BasicTypes.h"
//#include "FlagEnum.h"
#include <type_traits>
template <typename EnumType>
using _UNDERLYING_ENUM_T = typename std::underlying_type<EnumType>::type;

#    define DEFINE_FLAG_ENUM_OPERATORS(ENUMTYPE)                                                                                                                                                                              \
        extern "C++"                                                                                                                                                                                                          \
        {                                                                                                                                                                                                                     \
            inline ENUMTYPE&          operator|=(ENUMTYPE& a, ENUMTYPE b) throw() { return reinterpret_cast<ENUMTYPE&>(reinterpret_cast<_UNDERLYING_ENUM_T<ENUMTYPE>&>(a) |= static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(b)); } \
            inline ENUMTYPE&          operator&=(ENUMTYPE& a, ENUMTYPE b) throw() { return reinterpret_cast<ENUMTYPE&>(reinterpret_cast<_UNDERLYING_ENUM_T<ENUMTYPE>&>(a) &= static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(b)); } \
            inline ENUMTYPE&          operator^=(ENUMTYPE& a, ENUMTYPE b) throw() { return reinterpret_cast<ENUMTYPE&>(reinterpret_cast<_UNDERLYING_ENUM_T<ENUMTYPE>&>(a) ^= static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(b)); } \
            inline constexpr ENUMTYPE operator|(ENUMTYPE a, ENUMTYPE b) throw() { return static_cast<ENUMTYPE>(static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(a) | static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(b)); }                \
            inline constexpr ENUMTYPE operator&(ENUMTYPE a, ENUMTYPE b) throw() { return static_cast<ENUMTYPE>(static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(a) & static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(b)); }                \
            inline constexpr ENUMTYPE operator^(ENUMTYPE a, ENUMTYPE b) throw() { return static_cast<ENUMTYPE>(static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(a) ^ static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(b)); }                \
            inline constexpr ENUMTYPE operator~(ENUMTYPE a) throw() { return static_cast<ENUMTYPE>(~static_cast<_UNDERLYING_ENUM_T<ENUMTYPE>>(a)); }                                                                          \
        }


struct MouseState
{
    enum BUTTON_FLAGS : unsigned char
    {
        BUTTON_FLAG_NONE   = 0x00,
        BUTTON_FLAG_LEFT   = 0x01,
        BUTTON_FLAG_MIDDLE = 0x02,
        BUTTON_FLAG_RIGHT  = 0x04,
        BUTTON_FLAG_WHEEL  = 0x08
    };

    float      PosX        = -1;
    float      PosY        = -1;
    BUTTON_FLAGS ButtonFlags = BUTTON_FLAG_NONE;
    float      WheelDelta  = 0;
};
DEFINE_FLAG_ENUM_OPERATORS(MouseState::BUTTON_FLAGS)


enum class InputKeys
{
    Unknown = 0,
    MoveLeft,
    MoveRight,
    MoveForward,
    MoveBackward,
    MoveUp,
    MoveDown,
    Reset,
    ControlDown,
    ShiftDown,
    AltDown,
    ZoomIn,
    ZoomOut,
    TotalKeys
};

enum INPUT_KEY_STATE_FLAGS : unsigned char
{
    INPUT_KEY_STATE_FLAG_KEY_NONE     = 0x00,
    INPUT_KEY_STATE_FLAG_KEY_IS_DOWN  = 0x01,
    INPUT_KEY_STATE_FLAG_KEY_WAS_DOWN = 0x80
};
DEFINE_FLAG_ENUM_OPERATORS(INPUT_KEY_STATE_FLAGS)

class InputControllerBase
{
public:
    const MouseState& GetMouseState() const
    {
        return m_MouseState;
    }

    INPUT_KEY_STATE_FLAGS GetKeyState(InputKeys Key) const
    {
        return m_Keys[static_cast<size_t>(Key)];
    }

    bool IsKeyDown(InputKeys Key) const
    {
        return (GetKeyState(Key) & INPUT_KEY_STATE_FLAG_KEY_IS_DOWN) != 0;
    }

    void ClearState()
    {
        m_MouseState.WheelDelta = 0;

        for (unsigned int i = 0; i < static_cast<unsigned int>(InputKeys::TotalKeys); ++i)
        {
            auto& KeyState = m_Keys[i];
            if (KeyState & INPUT_KEY_STATE_FLAG_KEY_WAS_DOWN)
            {
                KeyState &= ~INPUT_KEY_STATE_FLAG_KEY_WAS_DOWN;
            }
        }
    }

protected:
    MouseState            m_MouseState;
    INPUT_KEY_STATE_FLAGS m_Keys[static_cast<size_t>(InputKeys::TotalKeys)] = {};
};
#include "InputControllerWin32.hpp"
using InputController = InputControllerWin32;