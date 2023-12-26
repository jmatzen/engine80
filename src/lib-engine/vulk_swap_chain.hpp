#pragma once

#include "engine80.hpp"
#include "vulk_swap_chain_support_details.hpp"
#include "vulk_result.hpp"
#include <vulkan/vulkan.h>
#include <span>

namespace qf::vulk
{
    class LogicalDevice;


    class SwapChain : public EnableSharedFromThis<SwapChain>
    {
        VkSwapchainKHR const swapChain{};
        weak<LogicalDevice> const logicalDevice{};

    public:
        /**
         * Constructor.
         * Initializes the SwapChain object with the given Vulkan device and surface.
         *
         * @param device The Vulkan logical device.
         * @param surface The Vulkan surface.
         */
        SwapChain(VkSwapchainKHR swapChain, LogicalDevice& device);

        /**
         * Destructor.
         * Cleans up the SwapChain object.
         */
        ~SwapChain();

        void dispose() override;

        /**
         * Getter method to retrieve the Vulkan swap chain handle.
         *
         * @return The Vulkan swap chain handle.
         */
        VkSwapchainKHR getSwapChain() const;

        /**
         * Getter method to retrieve the extent (width and height) of the swap chain images.
         *
         * @return The extent of the swap chain images.
         */
        VkExtent2D getExtent() const;

        /**
         * Getter method to retrieve the format of the swap chain images.
         *
         * @return The format of the swap chain images.
         */
        VkFormat getImageFormat() const;

        /**
         * Getter method to retrieve the vector of swap chain images.
         *
         * @return The vector of swap chain images.
         */
        const std::vector<VkImage>& getImages() const;

        /**
         * Method to create the swap chain using the specified Vulkan device and surface.
         *
         * @param device The Vulkan logical device.
         * @param surface The Vulkan surface.
         */
        static Expected<Box<SwapChain>> createSwapChain(LogicalDevice& device);

        /**
         * Method to destroy the swap chain using the specified Vulkan device.
         *
         * @param device The Vulkan logical device.
         */
        void destroySwapChain(VkDevice device);

        static Expected<SwapChainSupportDetails> querySwapChainSupport(VkPhysicalDevice, VkSurfaceKHR);


    private:
        /**
         * Helper method to choose the surface format for the swap chain.
         *
         * @param availableFormats The available surface formats.
         * @return The chosen surface format for the swap chain.
         */
        static Expected<VkSurfaceFormatKHR> chooseSwapSurfaceFormat(const std::span<VkSurfaceFormatKHR>& availableFormats);

        /**
         * Helper method to choose the presentation mode for the swap chain.
         *
         * @param availablePresentModes The available presentation modes.
         * @return The chosen presentation mode for the swap chain.
         */
        static VkPresentModeKHR chooseSwapPresentMode(const std::span<VkPresentModeKHR>& availablePresentModes);

        /**
         * Helper method to choose the extent (width and height) of the swap chain images.
         *
         * @param capabilities The capabilities of the surface.
         * @param windowWidth The width of the window.
         * @param windowHeight The height of the window.
         * @return The chosen extent of the swap chain images.
         */
        static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int windowWidth, int windowHeight);

        /**
         * Helper method to retrieve the images of the swap chain.
         *
         * @param device The Vulkan logical device.
         * @return The vector of swap chain images.
         */
        std::vector<VkImage> getSwapChainImages(VkDevice device);
    };
}
