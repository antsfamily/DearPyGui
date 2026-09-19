#pragma once

#include "mvItemRegistry.h"
#include <array>
#include <mvPlotting.h>
#include <implot3d_internal.h>


struct mvPlot3DConfig;
struct mvPlot3DAxisConfig;
struct mvScatter3DSeriesConfig;

namespace DearPyGui
{
    void fill_configuration_dict(const mvScatter3DSeriesConfig& inConfig, PyObject* outDict);
    void fill_configuration_dict(const mvPlot3DAxisConfig& inConfig, PyObject* outDict);
    void fill_configuration_dict(const mvPlot3DConfig& inConfig, PyObject* outDict);

    void set_configuration(PyObject* inDict, mvScatter3DSeriesConfig& outConfig);
    void set_configuration(PyObject* inDict, mvPlot3DAxisConfig& outConfig, mvAppItem& item);
    void set_configuration(PyObject* inDict, mvPlot3DConfig& outConfig);

    void set_positional_configuration(PyObject* inDict, mvScatter3DSeriesConfig& outConfig);

    void set_required_configuration(PyObject* inDict, mvPlot3DAxisConfig& outConfig);

    void draw_plot3d            (ImDrawList* drawlist, mvAppItem& item, mvPlot3DConfig& config);
    void draw_plot3d_axis       (ImDrawList* drawlist, mvAppItem& item, mvPlot3DAxisConfig& config);
    void draw_scatter3d_series  (ImDrawList* drawlist, mvAppItem& item, const mvScatter3DSeriesConfig& config);
}

struct mvScatter3DSeriesConfig : _mvBasicSeriesConfig
{
    std::string          marker = "s";
    std::vector<float>   size = {-1};
    std::vector<ImVec4> color;
    std::vector<ImU32>   pkcolor;
    float   filla = -1;
    int     stride = -1;
    ImPlot3DScatterFlags flags = ImPlot3DScatterFlags_None;
};

struct mvPlot3DAxisConfig
{
    ImPlot3DAxisFlags        flags = ImPlot3DAxisFlags_None;
    ImAxis3D                 axis = ImAxis3D_X;
    bool                     setLimits = false;
    bool                     setLimitsRange = false;
    bool                     setZoomRange = false;
    ImPlotScale              scale = ImPlotScale_Linear;
    std::string              formatter;
    ImVec2                   limits;
    ImVec2                   limits_actual;
    ImVec2                   constraints_range;
    ImVec2                   zoom_range;
    std::vector<std::string> labels;
    std::vector<double>      labelLocations;
    std::vector<const char*> clabels;
};

struct mvPlot3DConfig
{
    ImGuiMouseButton             pan;
    ImGuiKey                     pan_mod;
    ImGuiMouseButton             fit;
    ImGuiMouseButton             menu;
    ImGuiMouseButton             select;
    ImGuiKey                     select_mod;
    ImGuiMouseButton             select_cancel;
    ImGuiKey                     query_toggle_mod;
    ImGuiKey                     select_horz_mod;
    ImGuiKey                     select_vert_mod;
    ImGuiKey                     override_mod;
    ImGuiKey                     zoom_mod;
    float                        zoom_rate = 0.1f;
    bool                         query_enabled = true;
    mvColor                      query_color = mvColor(0.0f, 1.0f, 0.0f, 1.0f);
    int                          max_query_rects = 1;
    int                          min_query_rects = 1;

    ImPlot3DFlags                               _flags = ImPlot3DFlags_NoLegend;
    bool                                        _newColorMap = false;
    bool                                        _useColorMap = false;
    ImPlot3DColormap                            _colormap = ImPlot3DColormap_Deep;
    bool                                        _equalAspectRatios = false;
    bool                                        querying = false;
    bool                                        _fitDirty = false;
    bool                                        _axisfitDirty[ImAxis_COUNT] = { false, false, false, false, false, false };
    bool                                        localTime = false;
    bool                                        iSO8601 = false;
    bool                                        clock24Hour = false;
};


class mvScatter3DSeries : public mvAppItem
{
public:
    mvScatter3DSeriesConfig configData{};
    explicit mvScatter3DSeries(mvUUID uuid) : mvAppItem(uuid) {}
    void handleSpecificPositionalArgs(PyObject* dict) override { DearPyGui::set_positional_configuration(dict, configData); }
    void draw(ImDrawList* drawlist, float x, float y) override { DearPyGui::draw_scatter3d_series(drawlist, *this, configData); }
    void handleSpecificKeywordArgs(PyObject* dict) override { DearPyGui::set_configuration(dict, configData); }
    void getSpecificConfiguration(PyObject* dict) override { DearPyGui::fill_configuration_dict(configData, dict); }
    void setDataSource(mvUUID dataSource) override { DearPyGui::set_data_source(*this, dataSource, configData.value); }
    void* getValue() override { return &configData.value; }
    PyObject* getPyValue() override { return ToPyList(*configData.value); }
    void setPyValue(PyObject* value) override { *configData.value = ToVectVectDouble(value); }
};

class mvPlot3DAxis : public mvAppItem
{
public:
    mvPlot3DAxisConfig configData{};
    explicit mvPlot3DAxis(mvUUID uuid) : mvAppItem(uuid) {}
    void draw(ImDrawList* drawlist, float x, float y) override { DearPyGui::draw_plot3d_axis(drawlist, *this, configData); }
    void handleSpecificRequiredArgs(PyObject* dict) override { DearPyGui::set_required_configuration(dict, configData); }
    void handleSpecificKeywordArgs(PyObject* dict) override { DearPyGui::set_configuration(dict, configData, *this); }
    void getSpecificConfiguration(PyObject* dict) override { DearPyGui::fill_configuration_dict(configData, dict); }
};

class mvPlot3D : public mvAppItem
{
public:
    mvPlot3DConfig configData{};
    explicit mvPlot3D(mvUUID uuid) : mvAppItem(uuid) { config.width = config.height = -1; }
    void draw(ImDrawList* drawlist, float x, float y) override { DearPyGui::draw_plot3d(drawlist, *this, configData); }
    void handleSpecificKeywordArgs(PyObject* dict) override { DearPyGui::set_configuration(dict, configData); }
    void getSpecificConfiguration(PyObject* dict) override { DearPyGui::fill_configuration_dict(configData, dict); }
};