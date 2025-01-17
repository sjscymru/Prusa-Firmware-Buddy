#pragma once

#include <cstdint>
#include <FreeRTOS.h>
#include <event_groups.h>

/**
 * Code in this module handles task/component dependencies
 *
 * Provided tools enable definition of dependency masks, waiting for dependencies being satisfied and marking dependencies as provided.
 */

typedef uint8_t dependency_t;

/// Definition fo different dependencies
enum class ComponentDependencies {
    NETWORKING_READY_IDX = 2,
    FILESYSTEM_READY_IDX = 3,
    // To be continued...
};

/// Allow shifting
constexpr dependency_t operator<<(const dependency_t &a, const ComponentDependencies &b) {
    return a << static_cast<dependency_t>(b);
};

/// Definitions of dependecies for different tasks/components
static constexpr dependency_t DEFAULT_TASK_DEPS = 0; // Default task has no dependecies
static constexpr dependency_t EXAMPLE_TASK_DEPS = 1 << ComponentDependencies::NETWORKING_READY_IDX | 1 << ComponentDependencies::FILESYSTEM_READY_IDX;

// Needed for inline mthods being embedded to different compilation modules
extern EventGroupHandle_t components_ready;

/// Initialize component dependecy resolution
extern void components_init();

/**
 * Wait for dependecies of the task/component
 */
inline void wait_for_dependecies(const dependency_t dependecies) {
    if (dependecies) {
        xEventGroupWaitBits(components_ready, dependecies, pdFALSE, pdTRUE, portMAX_DELAY);
    }
}

/**
 * Mark particular dependecy as satisfied
 */
inline void provide_dependecy(const ComponentDependencies dependecy) {
    xEventGroupSetBits(components_ready, 1 << dependecy);
}
