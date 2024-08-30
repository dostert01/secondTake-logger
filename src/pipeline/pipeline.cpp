#include "pipeline.h"
#include <memory>
#include <iostream>
#include <fstream>

#include "../logger/logger.h"

namespace second_take {

#define UNDEFINED_JSON_DATA "undefined"
#define JSON_PROPERTY_PIPELINE_NAME "pipelineName"
#define JSON_PROPERTY_PIPELINE_STEPS "pipelineSteps"
#define JSON_PROPERTY_STEP_NAME "stepName"
#define JSON_PROPERTY_LIBRARY_NAME "libraryName"

std::unique_ptr<Pipeline> Pipeline::getInstance() {
    std::unique_ptr<Pipeline> instance = std::make_unique<Pipeline>();
    return instance;
}

std::optional<std::unique_ptr<Pipeline>> Pipeline::getInstance(const std::string configFilePath) {
    std::unique_ptr<Pipeline> instance = getInstance();
    try {
        instance.get()->loadPipelineConfig(configFilePath);
    } catch (const std::exception& e) {
        second_take::Logger::getInstance().error("Failed loading pipeline configuration from " + configFilePath + " : " + e.what());
        return std::nullopt;
    }
    return instance;
}

uint Pipeline::getCountOfPipelineSteps() {
    return pipelineSteps.size();
}

std::string Pipeline::getPipelineName() {
    return pipelineName;
}

void Pipeline::setPipelineName(const std::string& pipelineName) {
    this->pipelineName = pipelineName;
}

void Pipeline::loadPipelineConfig(const std::string& configFilePath) {
    try {
        std::ifstream jsonFile(configFilePath);
        json jsonData = json::parse(jsonFile);
        jsonFile.close();
        loadPipelineMetaData(jsonData);
        loadPipelineSteps(jsonData);
    } catch (const std::exception& e) {
        throw;
    }
}

void Pipeline::loadPipelineMetaData(const json& jsonData) {
    if(jsonData.contains(JSON_PROPERTY_PIPELINE_NAME)) {
        setPipelineName(jsonData.value(JSON_PROPERTY_PIPELINE_NAME, UNDEFINED_JSON_DATA));
    } else {
        throw PipelineException("Failed to load pipeline metadata. pipelineName not set");
    }
}

void Pipeline::loadPipelineSteps(const json& jsonData) {
    if(jsonData.contains(JSON_PROPERTY_PIPELINE_STEPS) && jsonData[JSON_PROPERTY_PIPELINE_STEPS].is_array()) {
        for (const auto& step : jsonData[JSON_PROPERTY_PIPELINE_STEPS]) {
            std::unique_ptr<PipelineStep> currentStep = std::make_unique<PipelineStep>();
            currentStep.get()->setStepName(step.value(JSON_PROPERTY_STEP_NAME, UNDEFINED_JSON_DATA));
            currentStep.get()->setLibraryName(step.value(JSON_PROPERTY_LIBRARY_NAME, UNDEFINED_JSON_DATA));
            if(currentStep.get()->isInitComplete()) {
                pipelineSteps.push_back(std::move(currentStep));
            } else {
                throw PipelineException("Failed to load pipeline step: Definition of pipeline steps in json might be incomplete.");
            }
        }
    } else {
        throw PipelineException("Failed to load pipeline step: No pipeline steps array found in json.");
    }
}

//-------------------------------------------------------------------
void PipelineStep::setStepName(const std::string stepName) {
    this->stepName = stepName;
}

void PipelineStep::setLibraryName(const std::string libraryName) {
    this->libraryName = libraryName;
}

bool PipelineStep::isInitComplete() {
    return (stepName != UNDEFINED_JSON_DATA) && (libraryName != UNDEFINED_JSON_DATA);
}

//-------------------------------------------------------------------
PipelineException::PipelineException(const std::string& message) : std::exception() {
    this->message = message;
}

const char* PipelineException::what() const noexcept {
    return (char*)message.c_str();
}



}