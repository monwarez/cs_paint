/***********************************************************************
*
* Copyright (c) 2019 Barbara Geller
* Copyright (c) 2019 Ansel Sermersheim
*
* This file is part of CsPaint.
*
* CsPaint is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
*
* CopperSpice is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#include <cspaint_renderpass.h>

#include <cspaint_device.h>
#include <cspaint_util.h>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::renderpass>());

CsPaint::renderpass::renderpass(std::shared_ptr<const device> device, vk::UniqueRenderPass pass)
    : m_device(device), m_vk_renderpass(std::move(pass))
{
}
