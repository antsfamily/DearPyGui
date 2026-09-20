#include "mvPyUtils.h"
#pragma hdrstop

#include "mvPlotting3D.h"

#include "mvCore.h"
#include "mvContext.h"
#include "mvItemRegistry.h"
#include "mvFontItems.h"
#include "mvThemes.h"
#include "mvContainers.h"
#include "mvTextureItems.h"
#include "mvItemHandlers.h"

#include <utility>



void
DearPyGui::draw_plot3d(ImDrawList* drawlist, mvAppItem& item, mvPlot3DConfig& config)
{
	if (!item.config.show)
		return;

	// cache old cursor position
	ImVec2 previousCursorPos = ImGui::GetCursorPos();

	// set cursor position if user set
	if (item.info.dirtyPos)
		ImGui::SetCursorPos(item.state.pos);

	// update widget's position state
	item.state.pos = { ImGui::GetCursorPosX(), ImGui::GetCursorPosY() };

	// push font if a font object is attached
	if (item.font)
		static_cast<mvFont*>(item.font.get())->pushFont();

	// themes
	apply_local_theming(&item);

	// // Must do this because these items are not avalable as style items
	// // these are here because they need to be applied every plot
	// ImPlot::GetStyle().UseLocalTime = config.localTime;
	// ImPlot::GetStyle().UseISO8601 = config.iSO8601;
	// ImPlot::GetStyle().Use24HourClock = config.clock24Hour;

	// if (config._newColorMap)
	// {
	// 	ImPlot::BustColorCache(item.info.internalLabel.c_str());
	// 	config._newColorMap = false;
	// }

	// if (config._useColorMap)
	// 	ImPlot::PushColormap(config._colormap);

	// // custom input mapping
	// ImPlot::GetInputMap().Pan = config.pan;
	// ImPlot::GetInputMap().Fit = config.fit;
	// ImPlot::GetInputMap().Select = config.select;
	// ImPlot::GetInputMap().SelectCancel = config.select_cancel;
	// ImPlot::GetInputMap().Menu = config.menu;
	// ImPlot::GetInputMap().ZoomRate = config.zoom_rate;

	// if (config.pan_mod != ImPlot::GetInputMap().PanMod) ImPlot::GetInputMap().PanMod = config.pan_mod;
	// if (config.select_mod != ImPlot::GetInputMap().SelectMod) ImPlot::GetInputMap().SelectMod = config.select_mod;
	// if (config.zoom_mod != ImPlot::GetInputMap().ZoomMod) ImPlot::GetInputMap().ZoomMod = config.zoom_mod;
	// if (config.override_mod != ImPlot::GetInputMap().OverrideMod) ImPlot::GetInputMap().OverrideMod = config.override_mod;
	// if (config.select_horz_mod != ImPlot::GetInputMap().SelectHorzMod) ImPlot::GetInputMap().SelectHorzMod = config.select_horz_mod;
	// if (config.select_vert_mod != ImPlot::GetInputMap().SelectVertMod) ImPlot::GetInputMap().SelectVertMod = config.select_vert_mod;

	// if (config._fitDirty)
	// {
	// 	// This must be called before BeginPlot
	// 	for(int i = 0; i < ImAxis3D_COUNT; i++) {
	// 		if (config._axisfitDirty[i] == true) {
	// 			ImPlot3D::SetNextAxisToFit(i);
	// 			config._axisfitDirty[i] = false;
	// 		}
	// 	}
	// 	config._fitDirty = false;
	// }
	ImPlot3D::PushColormap("Jet");
	if (ImPlot3D::BeginPlot(item.info.internalLabel.c_str(), ImVec2((float)item.config.width, (float)item.config.height), config._flags))
	{	

		// gives axes change to make changes to ticks, limits, etc.
		ImAxis3D next_y_axis = ImAxis_Y1;
		for (auto& child : item.childslots[1])
		{
			// skip item if it's not shown
			if (!child->config.show)
				continue;

			if (child->type == mvAppItemType::mvPlot3DAxis)
			{
				mvPlot3DAxis* axis = static_cast<mvPlot3DAxis*>(child.get());
				ImAxis3D_ id_axis = static_cast<ImAxis3D_>(axis->configData.axis);

				// // auto-assigning additional Y axes for compatibility with DPG 1.11 and earlier versions
				auto flags = axis->configData.flags;
				// if (id_axis == ImAxis_Y1)
				// {
				// 	if (axis->configData.axis < next_y_axis)
				// 	{
				// 		id_axis = static_cast<ImAxis3D_>(next_y_axis);
				// 		flags |= ImPlotAxisFlags_Opposite;
				// 		axis->configData.axis = next_y_axis;
				// 	}
				// 	++next_y_axis;
				// }

				ImPlot3D::SetupAxis(id_axis, axis->config.specifiedLabel.c_str(), flags);
				if (axis->configData.setLimits)
					ImPlot3D::SetupAxisLimits(id_axis, axis->configData.limits.x, axis->configData.limits.y, ImGuiCond_Always);

				// if (!axis->configData.formatter.empty())
				// 	ImPlot3D::SetupAxisFormat(id_axis, axis->configData.formatter.c_str());

				ImPlot3D::SetupAxisScale(id_axis, axis->configData.scale);

				if (axis->configData.setLimitsRange)
					ImPlot3D::SetupAxisLimitsConstraints(id_axis, axis->configData.constraints_range.x, axis->configData.constraints_range.y);
				if (axis->configData.setZoomRange)
					ImPlot3D::SetupAxisZoomConstraints(id_axis, axis->configData.zoom_range.x, axis->configData.zoom_range.y);

				if (!axis->configData.labels.empty())
				{
					// TODO: Checks (from original dpg)
					ImPlot3D::SetupAxisTicks(id_axis, axis->configData.labelLocations.data(), (int)axis->configData.labels.size(), axis->configData.clabels.data());
				}
			}
			else
				child->customAction();
		}
		auto context = ImPlot3D::GetCurrentContext();

		// ImGuiIO& IO = ImGui::GetIO();
		// // Note: we can't use `config.querying` here because in the frame when
		// // the query modifier gets pressed, `querying` is still false but we already
		// // need to disable `OverrideMod`.
		// if (ImHasFlag(IO.KeyMods, config.query_toggle_mod) &&
		// 	(ImGui::IsMouseDown(config.select) || ImGui::IsMouseReleased(config.select)))
		// {
		// 	// Preventing ImPlot from getting stuck on selection if override modifier
		// 	// is pressed (e.g. when the override mod is the same as query toggle mod).
		// 	ImPlot::GetInputMap().OverrideMod = ImGuiMod_None;
		// }
		// else
		// 	ImPlot::GetInputMap().OverrideMod = config.override_mod;

		// bool query_dirty = false;
		// if (config.query_enabled && config.querying && ImGui::IsMouseReleased(config.select))
		// {
		// 	if (config.max_query_rects != 0 && config.rects.size() >= config.max_query_rects)
		// 		config.rects.pop_back();
		// 	config.rects.push_back(config.query_rect);
		// 	config.querying = false;
		// 	// Prevent ImPlot from handling mouse release on its own. This will block
		// 	// input handling in the current frame (later we'll reset OverrideMod).
		// 	ImPlot::GetInputMap().OverrideMod = IO.KeyMods;
		// 	// Note: this will lock the setup and might therefore skip changes
		// 	// to the legend, drag points, and lines in this frame.  Nothing we
		// 	// can do about that, really.
		// 	ImPlot::CancelPlotSelection();
		// 	// We've updated the list, let's report this
		// 	query_dirty = true;
		// }

		// legend, drag point and lines
		for (auto& child : item.childslots[0]) // Using "ImPlot::GetPlotRectPos()" here trigger an assert
			child->draw(drawlist, context->CurrentPlot->PlotRect.Min.x, context->CurrentPlot->PlotRect.Min.y);

		// axes
		for (auto& child : item.childslots[1])
			child->draw(drawlist, ImPlot3D::GetPlotRectPos().x, ImPlot3D::GetPlotRectPos().y);

		// ImPlot::PushPlotClipRect();

		// ImPlot3D::SetAxis(ImAxis_Y1);

		// drawings
		for (auto& child : item.childslots[2])
		{
			// skip item if it's not shown
			if (!child->config.show)
				continue;

			// item->draw(ImPlot3D::GetPlotDrawList(), ImPlot3D::GetPlotRectPos().x, ImPlot3D::GetPlotRectPos().y);
			child->draw(ImPlot3D::GetPlotDrawList(), 0.0f, 0.0f);

			UpdateAppItemState(child->state);
		}


		// update state
		config._flags = context->CurrentPlot->Flags;

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		{

			// update mouse
			ImVec2 mousePos = ImGui::GetMousePos();
			ImVec2 windowPos = ImGui::GetWindowPos();
			GContext->input.mousePos.x = (int)(mousePos.x - windowPos.x);
			GContext->input.mousePos.y = (int)(mousePos.y - windowPos.y);

			GContext->activeWindow = item.uuid;

		}

		// TODO: find a better way to handle this
		// We could use std::find_if from <algorithm> but it'd require item.childslots[0] to be a std::vector
		for (auto& child : item.childslots[0])
		{
			if (child->type == mvAppItemType::mvPlotLegend)
			{
				auto legend = static_cast<mvPlotLegend*>(child.get());
				legend->configData.legendLocation = context->CurrentPlot->Items.Legend.Location;
				legend->configData.flags = context->CurrentPlot->Items.Legend.Flags;
				break;
			}
		}

		ImPlot3D::EndPlot();

	}
	ImPlot3D::PopColormap();

	// set cursor position to cached position
	if (item.info.dirtyPos)
		DearPyGui::RestoreImGuiCursor(previousCursorPos);

	// ImPlot3D::GetInputMap() = config._originalMap;

	UpdateAppItemState(item.state);

	if (item.font)
	{
		ImGui::PopFont();
	}

	if (item.theme)
	{
		item.theme->pop_theme_components();
	}

	if (item.handlerRegistry)
		item.handlerRegistry->checkEvents(&item.state);

	// drag drop
	for (auto& child : item.childslots[3])
		child->draw(nullptr, ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
}

void
DearPyGui::draw_plot3d_axis(ImDrawList* drawlist, mvAppItem& item, mvPlot3DAxisConfig& config)
{
	if (!item.config.show)
		return;

	// todo: add check
	// ImPlot3D::SetAxis(config.axis);

	for (auto& item : item.childslots[1])
		item->draw(drawlist, ImPlot3D::GetPlotRectPos().x, ImPlot3D::GetPlotRectPos().y);

	// // x axis
	// if (config.axis <= ImAxis3D_X)
	// {
	// 	auto plotLimits = ImPlot3D::GetPlotLimits(config.axis, IMPLOT_AUTO);
	// 	config.limits_actual.x = (float)plotLimits.X.Min;
	// 	config.limits_actual.y = (float)plotLimits.X.Max;
	// }

	// // y axis
	// else
	// {
	// 	auto plotLimits = ImPlot3D::GetPlotLimits(IMPLOT_AUTO, config.axis);
	// 	config.limits_actual.x = (float)plotLimits.Y.Min;
	// 	config.limits_actual.y = (float)plotLimits.Y.Max;
	// }

	config.flags = ImPlot3D::GetCurrentContext()->CurrentPlot->Axes[config.axis].Flags;

	UpdateAppItemState(item.state);

	if (item.font)
		ImGui::PopFont();

	if (item.theme)
		static_cast<mvTheme*>(item.theme.get())->customAction();

	// if (item.config.dropCallback)
	// {
	// 	ScopedID id(item.uuid);
	// 	if (ImPlot3D::BeginDragDropTargetAxis(config.axis))
	// 	{
	// 		check_drop_event(&item);
	// 		ImPlot3D::EndDragDropTarget();
	// 	}
	// }
}

void
DearPyGui::draw_scatter3d_series(ImDrawList* drawlist, mvAppItem& item, const mvScatter3DSeriesConfig& config)
{
	//-----------------------------------------------------------------------------
	// pre draw
	//-----------------------------------------------------------------------------
	if (!item.config.show)
		return;

	// push font if a font object is attached
	if (item.font)
		static_cast<mvFont*>(item.font.get())->pushFont();

	// themes
	apply_local_theming(&item);

	//-----------------------------------------------------------------------------
	// draw
	//-----------------------------------------------------------------------------
	{

		static const std::vector<double>* xptr;
		static const std::vector<double>* yptr;
		static const std::vector<double>* zptr;
		static std::vector<ImU32> pkcolor;

		xptr = &(*config.value.get())[0];
		yptr = &(*config.value.get())[1];
		zptr = &(*config.value.get())[2];

		ImPlot3DSpec spec;

		if (config.marker == "s")
			spec.Marker = ImPlot3DMarker_Square;
		else if (config.marker == "c")
			spec.Marker = ImPlot3DMarker_Circle;
		else if (config.marker == "d")
			spec.Marker = ImPlot3DMarker_Diamond;
		else if (config.marker == "p")
			spec.Marker = ImPlot3DMarker_Plus;
		else if (config.marker == "x")
			spec.Marker = ImPlot3DMarker_Cross;
		else if (config.marker == "a")
			spec.Marker = ImPlot3DMarker_Asterisk;
		else if (config.marker == "up")
			spec.Marker = ImPlot3DMarker_Up;
		else if (config.marker == "down")
			spec.Marker = ImPlot3DMarker_Down;
		else if (config.marker == "left")
			spec.Marker = ImPlot3DMarker_Left;
		else if (config.marker == "right")
			spec.Marker = ImPlot3DMarker_Right;
		else
			spec.Marker = ImPlot3DMarker_Auto;

		if (!config.size.empty())
		{
			if (config.size.size() == 1)
				spec.MarkerSize = config.size[0];
			else if (config.size.size() == xptr->size())
				spec.MarkerSizes = const_cast<float*>(config.size.data());
		}

		// Handle per-point colors - prefer pkcolor if available
		if (!config.pkcolor.empty())
		{
			// Use packed colors directly (ImU32 format)
			if (config.pkcolor.size() == 1)
			{
				// Convert single ImU32 to ImVec4
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(config.pkcolor[0]);
				spec.MarkerLineColor = color;
				spec.MarkerFillColor = color;
			}
			else if (config.pkcolor.size() >= xptr->size())
			{
				spec.MarkerLineColors = const_cast<ImU32*>(config.pkcolor.data());
				spec.MarkerFillColors = spec.MarkerLineColors;
			}
		}
		else if (!config.color.empty())
		{
			if (config.color.size() == 1)
			{
				spec.MarkerLineColor = config.color[0];
				spec.MarkerFillColor = config.color[0];
			}
			else if (config.color.size() >= xptr->size())
			{
				// Convert ImVec4 to packed colors (ImU32)
				pkcolor.clear();
				pkcolor.reserve(config.color.size());
				for (const auto& color : config.color)
					pkcolor.push_back(ImGui::GetColorU32(color));
				spec.MarkerLineColors = pkcolor.data();
				spec.MarkerFillColors = pkcolor.data();
			}
		}

		spec.FillAlpha = config.filla;

		ImPlot3D::PlotScatter(item.info.internalLabel.c_str(), xptr->data(), yptr->data(), zptr->data(), (int)xptr->size(), spec);
	}

	//-----------------------------------------------------------------------------
	// update state
	//   * only update if applicable
	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	// post draw
	//-----------------------------------------------------------------------------
	//
	// pop font off stack
	if (item.font)
		ImGui::PopFont();

	// handle popping themes
	cleanup_local_theming(&item);
}



void
DearPyGui::set_required_configuration(PyObject* inDict, mvPlot3DAxisConfig& outConfig)
{
	if (!VerifyRequiredArguments(GetParsers()[GetEntityCommand(mvAppItemType::mvPlot3DAxis)], inDict))
		return;

	outConfig.axis = ToInt(PyTuple_GetItem(inDict, 0));
}


void
DearPyGui::set_configuration(PyObject* inDict, mvPlot3DAxisConfig& outConfig, mvAppItem& item)
{
	if (inDict == nullptr)
		return;

	if (PyObject* item = PyDict_GetItemString(inDict, "scale")) outConfig.scale = ToInt(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "tick_format")) outConfig.formatter = ToString(item);

	// Legacy
	if (PyObject* item = PyDict_GetItemString(inDict, "log_scale")) outConfig.scale = (ToBool(item) ? ImPlotScale_Log10 : outConfig.scale);
	if (PyObject* item = PyDict_GetItemString(inDict, "time")) outConfig.scale = (ToBool(item) ? ImPlotScale_Time : outConfig.scale);

	// helper for bit flipping
	auto flagop = [inDict](const char* keyword, int flag, int& flags)
	{
		if (PyObject* item = PyDict_GetItemString(inDict, keyword)) ToBool(item) ? flags |= flag : flags &= ~flag;
	};

	// axis flags
	flagop("no_label", ImPlot3DAxisFlags_NoLabel, outConfig.flags);
	flagop("no_gridlines", ImPlot3DAxisFlags_NoGridLines, outConfig.flags);
	flagop("no_tick_marks", ImPlot3DAxisFlags_NoTickMarks, outConfig.flags);
	flagop("no_tick_labels", ImPlot3DAxisFlags_NoTickLabels, outConfig.flags);
	// flagop("no_initial_fit", ImPlot3DAxisFlags_NoInitialFit, outConfig.flags);
	// flagop("no_menus", ImPlot3DAxisFlags_NoMenus, outConfig.flags);
	// flagop("no_side_switch", ImPlot3DAxisFlags_NoSideSwitch, outConfig.flags);
	// flagop("no_highlight", ImPlot3DAxisFlags_NoHighlight, outConfig.flags);
	// flagop("opposite", ImPlot3DAxisFlags_Opposite, outConfig.flags);
	// flagop("foreground_grid", ImPlot3DAxisFlags_Foreground, outConfig.flags);
	flagop("invert", ImPlot3DAxisFlags_Invert, outConfig.flags);
	flagop("auto_fit", ImPlot3DAxisFlags_AutoFit, outConfig.flags);
	// flagop("range_fit", ImPlot3DAxisFlags_RangeFit, outConfig.flags);
	flagop("pan_stretch", ImPlot3DAxisFlags_PanStretch, outConfig.flags);
	flagop("lock_min", ImPlot3DAxisFlags_LockMin, outConfig.flags);
	flagop("lock_max", ImPlot3DAxisFlags_LockMax, outConfig.flags);

	if (item.info.shownLastFrame)
	{
		item.info.shownLastFrame = false;
		if (auto plot = static_cast<mvPlot*>(item.info.parentPtr))
			plot->configData._flags &= ~ImPlot3DFlags_NoLegend;
		item.config.show = true;
	}

	if (item.info.hiddenLastFrame)
	{
		item.info.hiddenLastFrame = false;
		if (auto plot = static_cast<mvPlot*>(item.info.parentPtr))
			plot->configData._flags |= ImPlot3DFlags_NoLegend;
		item.config.show = false;
	}
}



void
DearPyGui::fill_configuration_dict(const mvPlot3DAxisConfig& inConfig, PyObject* outDict)
{
	if (outDict == nullptr)
		return;

	PyDict_SetItemString(outDict, "scale", mvPyObject(ToPyInt(inConfig.scale)));
	PyDict_SetItemString(outDict, "tick_format", mvPyObject(ToPyString(inConfig.formatter)));
	// Legacy
	PyDict_SetItemString(outDict, "log_scale", mvPyObject(ToPyBool(inConfig.scale == ImPlot3DScale_Log10)));
	PyDict_SetItemString(outDict, "time", mvPyObject(ToPyBool(inConfig.scale == ImPlotScale_Time)));

	// helper to check and set bit
	auto checkbitset = [outDict](const char* keyword, int flag, const int& flags)
	{
		mvPyObject py_result = ToPyBool(flags & flag);
		PyDict_SetItemString(outDict, keyword, py_result);
	};

	// plot flags
	checkbitset("no_label", ImPlot3DAxisFlags_NoLabel, inConfig.flags);
	checkbitset("no_gridlines", ImPlot3DAxisFlags_NoGridLines, inConfig.flags);
	checkbitset("no_tick_marks", ImPlot3DAxisFlags_NoTickMarks, inConfig.flags);
	checkbitset("no_tick_labels", ImPlot3DAxisFlags_NoTickLabels, inConfig.flags);
	// checkbitset("no_initial_fit", ImPlot3DAxisFlags_NoInitialFit, inConfig.flags);
	// checkbitset("no_menus", ImPlot3DAxisFlags_NoMenus, inConfig.flags);
	// checkbitset("no_side_switch", ImPlot3DAxisFlags_NoSideSwitch, inConfig.flags);
	// checkbitset("no_highlight", ImPlot3DAxisFlags_NoHighlight, inConfig.flags);
	// checkbitset("opposite", ImPlot3DAxisFlags_Opposite, inConfig.flags);
	// checkbitset("foreground_grid", ImPlot3DAxisFlags_Foreground, inConfig.flags);
	checkbitset("invert", ImPlot3DAxisFlags_Invert, inConfig.flags);
	checkbitset("auto_fit", ImPlot3DAxisFlags_AutoFit, inConfig.flags);
	// checkbitset("range_fit", ImPlot3DAxisFlags_RangeFit, inConfig.flags);
	checkbitset("pan_stretch", ImPlot3DAxisFlags_PanStretch, inConfig.flags);
	checkbitset("lock_min", ImPlot3DAxisFlags_LockMin, inConfig.flags);
	checkbitset("lock_max", ImPlot3DAxisFlags_LockMax, inConfig.flags);
}


void
DearPyGui::set_configuration(PyObject* inDict, mvPlot3DConfig& outConfig)
{
	if (inDict == nullptr)
		return;

	if (PyObject* item = PyDict_GetItemString(inDict, "pan_button")) outConfig.pan = ToInt(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "fit_button")) outConfig.fit = ToInt(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "context_menu_button")) outConfig.menu = ToInt(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "box_select_button")) outConfig.select = ToInt(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "box_select_cancel_button")) outConfig.select_cancel = ToInt(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "box_select_mod_button")) outConfig.select_mod = static_cast<ImGuiKey>(ToInt(item));
	if (PyObject* item = PyDict_GetItemString(inDict, "query_toggle_mod")) outConfig.query_toggle_mod = static_cast<ImGuiKey>(ToInt(item));
	if (PyObject* item = PyDict_GetItemString(inDict, "horizontal_mod")) outConfig.select_horz_mod = static_cast<ImGuiKey>(ToInt(item));
	if (PyObject* item = PyDict_GetItemString(inDict, "vertical_mod")) outConfig.select_vert_mod = static_cast<ImGuiKey>(ToInt(item));
	if (PyObject* item = PyDict_GetItemString(inDict, "use_local_time")) outConfig.localTime = ToBool(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "use_ISO8601")) outConfig.iSO8601 = ToBool(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "use_24hour_clock")) outConfig.clock24Hour = ToBool(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "pan_mod")) outConfig.pan_mod = static_cast<ImGuiKey>(ToInt(item));
	if (PyObject* item = PyDict_GetItemString(inDict, "override_mod")) outConfig.override_mod = static_cast<ImGuiKey>(ToInt(item));
	if (PyObject* item = PyDict_GetItemString(inDict, "zoom_mod")) outConfig.zoom_mod = static_cast<ImGuiKey>(ToInt(item));
	if (PyObject* item = PyDict_GetItemString(inDict, "zoom_rate")) outConfig.zoom_rate = ToFloat(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "query")) outConfig.query_enabled = ToBool(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "query_color")) outConfig.query_color = ToColor(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "min_query_rects")) outConfig.min_query_rects = ToInt(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "max_query_rects")) outConfig.max_query_rects = ToInt(item);
	// helper for bit flipping
	auto flagop = [inDict](const char* keyword, int flag, int& flags)
	{
		if (PyObject* item = PyDict_GetItemString(inDict, keyword)) ToBool(item) ? flags |= flag : flags &= ~flag;
	};

	// plot flags
	flagop("no_title", ImPlotFlags_NoTitle, outConfig._flags);
	flagop("no_menus", ImPlotFlags_NoMenus, outConfig._flags);
	flagop("no_box_select", ImPlotFlags_NoBoxSelect, outConfig._flags);
	flagop("no_mouse_pos", ImPlotFlags_NoMouseText, outConfig._flags);
	flagop("crosshairs", ImPlotFlags_Crosshairs, outConfig._flags);
	flagop("equal_aspects", ImPlotFlags_Equal, outConfig._flags);
	flagop("no_inputs", ImPlotFlags_NoInputs, outConfig._flags);
	flagop("no_frame", ImPlotFlags_NoFrame, outConfig._flags);	
	// flagop("canvas_only", ImPlotFlags_CanvasOnly, outConfig._flags);
}


void
DearPyGui::fill_configuration_dict(const mvPlot3DConfig& inConfig, PyObject* outDict)
{
	if (outDict == nullptr)
		return;

	PyDict_SetItemString(outDict, "pan_button", mvPyObject(ToPyInt(inConfig.pan)));
	PyDict_SetItemString(outDict, "pan_mod", mvPyObject(ToPyInt(inConfig.pan_mod)));
	PyDict_SetItemString(outDict, "fit_button", mvPyObject(ToPyInt(inConfig.fit)));
	PyDict_SetItemString(outDict, "context_menu_button", mvPyObject(ToPyInt(inConfig.menu)));
	PyDict_SetItemString(outDict, "box_select_button", mvPyObject(ToPyInt(inConfig.select)));
	PyDict_SetItemString(outDict, "box_select_mod", mvPyObject(ToPyInt(inConfig.select_mod)));
	PyDict_SetItemString(outDict, "box_select_cancel_button", mvPyObject(ToPyInt(inConfig.select_cancel)));
	PyDict_SetItemString(outDict, "query_toggle_mod", mvPyObject(ToPyInt(inConfig.query_toggle_mod)));
	PyDict_SetItemString(outDict, "horizontal_mod", mvPyObject(ToPyInt(inConfig.select_horz_mod)));
	PyDict_SetItemString(outDict, "vertical_mod", mvPyObject(ToPyInt(inConfig.select_vert_mod)));
	PyDict_SetItemString(outDict, "override_mod", mvPyObject(ToPyInt(inConfig.override_mod)));
	PyDict_SetItemString(outDict, "zoom_mod", mvPyObject(ToPyInt(inConfig.zoom_mod)));
	PyDict_SetItemString(outDict, "zoom_rate", mvPyObject(ToPyFloat(inConfig.zoom_rate)));
	// PyDict_SetItemString(outDict, "use_local_time", mvPyObject(ToPyBool(inConfig.localTime)));
	// PyDict_SetItemString(outDict, "use_ISO8601", mvPyObject(ToPyBool(inConfig.iSO8601)));
	// PyDict_SetItemString(outDict, "use_24hour_clock", mvPyObject(ToPyBool(inConfig.clock24Hour)));
	// PyDict_SetItemString(outDict, "query", mvPyObject(ToPyBool(inConfig.query_enabled)));
	// PyDict_SetItemString(outDict, "query_color", mvPyObject(ToPyColor(inConfig.query_color)));
	// PyDict_SetItemString(outDict, "min_query_rects", mvPyObject(ToPyInt(inConfig.min_query_rects)));
	// PyDict_SetItemString(outDict, "max_query_rects", mvPyObject(ToPyInt(inConfig.max_query_rects)));

	// helper to check and set bit
	auto checkbitset = [outDict](const char* keyword, int flag, const int& flags)
	{
		mvPyObject py_result = ToPyBool(flags & flag);
		PyDict_SetItemString(outDict, keyword, py_result);
	};

	// plot flags
	checkbitset("no_title", ImPlot3DFlags_NoTitle, inConfig._flags);
	checkbitset("no_menus", ImPlot3DFlags_NoMenus, inConfig._flags);
	// checkbitset("no_box_select", ImPlot3DFlags_NoBoxSelect, inConfig._flags);
	checkbitset("no_mouse_pos", ImPlot3DFlags_NoMouseText, inConfig._flags);
	// checkbitset("crosshairs", ImPlot3DFlags_Crosshairs, inConfig._flags);
	checkbitset("equal_aspects", ImPlot3DFlags_Equal, inConfig._flags);
	checkbitset("no_inputs", ImPlot3DFlags_NoInputs, inConfig._flags);
	// checkbitset("no_frame", ImPlot3DFlags_NoFrame, inConfig._flags);
	// checkbitset("canvas_only", ImPlot3DFlags_CanvasOnly, inConfig._flags);
}

void
DearPyGui::set_positional_configuration(PyObject* inDict, mvScatter3DSeriesConfig& outConfig)
{
	if (!VerifyRequiredArguments(GetParsers()[GetEntityCommand(mvAppItemType::mvScatter3DSeries)], inDict))
		return;

	for(int i = 0; i < PyTuple_Size(inDict); i++)
		(*outConfig.value)[i] = ToDoubleVect(PyTuple_GetItem(inDict, i));
}



void
DearPyGui::set_configuration(PyObject* inDict, mvScatter3DSeriesConfig& outConfig)
{
	if (inDict == nullptr)
		return;

	if (PyObject* item = PyDict_GetItemString(inDict, "x")) { (*outConfig.value)[0] = ToDoubleVect(item); }
	if (PyObject* item = PyDict_GetItemString(inDict, "y")) { (*outConfig.value)[1] = ToDoubleVect(item); }
	if (PyObject* item = PyDict_GetItemString(inDict, "z")) { (*outConfig.value)[2] = ToDoubleVect(item); }

	if (PyObject* item = PyDict_GetItemString(inDict, "marker")) outConfig.marker = ToString(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "size")) outConfig.size = ToFloatVect(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "color"))
	{
		outConfig.color.clear();
		if (PySequence_Check(item))
		{
			Py_ssize_t size = PySequence_Size(item);
			outConfig.color.reserve(size);
			for (Py_ssize_t i = 0; i < size; i++)
			{
				PyObject* colorItem = PySequence_GetItem(item, i);
				outConfig.color.push_back(ToVec4(colorItem));
				Py_DECREF(colorItem);
			}
		}
	}
	if (PyObject* item = PyDict_GetItemString(inDict, "filla")) outConfig.filla = ToFloat(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "stride")) outConfig.stride = ToInt(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "pkcolor"))
	{
		outConfig.pkcolor.clear();
		if (PySequence_Check(item))
		{
			Py_ssize_t size = PySequence_Size(item);
			outConfig.pkcolor.reserve(size);
			for (Py_ssize_t i = 0; i < size; i++)
			{
				PyObject* colorItem = PySequence_GetItem(item, i);
				outConfig.pkcolor.push_back((ImU32)ToLong(colorItem));
				Py_DECREF(colorItem);
			}
		}
	}

	// helper for bit flipping
	auto flagop = [inDict](const char* keyword, int flag, int& flags)
	{
		if (PyObject* item = PyDict_GetItemString(inDict, keyword)) ToBool(item) ? flags |= flag : flags &= ~flag;
	};

	// flags
	flagop("no_clip", ImPlotScatterFlags_NoClip, outConfig.flags);
}


void
DearPyGui::fill_configuration_dict(const mvScatter3DSeriesConfig& inConfig, PyObject* outDict)
{
	if (outDict == nullptr)
		return;

	// helper to check and set bit
	auto checkbitset = [outDict](const char* keyword, int flag, const int& flags)
	{
		PyDict_SetItemString(outDict, keyword, mvPyObject(ToPyBool(flags & flag)));
	};

	// flags
	checkbitset("no_clip", ImPlotScatterFlags_NoClip, inConfig.flags);
}