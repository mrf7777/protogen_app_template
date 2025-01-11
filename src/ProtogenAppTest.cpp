#include <protogen/IProtogenApp.hpp>
#include <protogen/IProportionProvider.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IAttributeStore.hpp>
#include <protogen/StandardAttributeStore.hpp>
#include <cmake_vars.h>

#include <cmath>
#include <thread>

using namespace protogen;

class ProtogenAppTest : public protogen::IProtogenApp {
public:
    ProtogenAppTest()
        : m_deviceResolution(Resolution(0, 0)),
        m_attributes(std::shared_ptr<StandardAttributeStore>(new StandardAttributeStore())),
        m_mouthProvider(nullptr),
        m_active(false),
        m_initialized(false),
        m_webServerThread(),
        m_webServerPort(-1),
        m_resourcesDirectory()
    {
        using namespace protogen::attributes;
        using Access = protogen::attributes::IAttributeStore::Access;
        m_attributes->adminSetAttribute(ATTRIBUTE_ID, PROTOGEN_APP_ID, Access::Read);
        m_attributes->adminSetAttribute(ATTRIBUTE_NAME, "Protogen App Test", Access::Read);
        m_attributes->adminSetAttribute(ATTRIBUTE_DESCRIPTION, "This is a demo protogen app that is a simple template for education.", Access::Read);
        m_attributes->adminSetAttribute(ATTRIBUTE_THUMBNAIL, "/static/thumbnail.png", Access::Read);
        m_attributes->adminSetAttribute(ATTRIBUTE_MAIN_PAGE, "/static/index.html", Access::Read);
        m_attributes->adminSetAttribute(ATTRIBUTE_HOME_PAGE, "https://github.com/mrf7777/protogen_app_template", Access::Read);
    }

    bool sanityCheck([[maybe_unused]] std::string& errorMessage) const override {
        return true;
    }

    void initialize() override {
        m_webServerThread = std::thread([this](){
            using httplib::Request, httplib::Response;
            auto server = httplib::Server();

            server.set_mount_point("/static", m_resourcesDirectory + "/static");

            server.Get("/home", [](const Request&, Response& res){ res.set_content("This is some page.", "text/plain"); });
            server.Get("/hello", [](const Request&, Response& res){ res.set_content("Hello!", "text/plain"); });
            server.Get("/hello/website", [](const Request&, Response& res){ res.set_content("Hello, website!.", "text/plain"); });

            m_webServerPort = server.bind_to_any_port("0.0.0.0");
            server.listen_after_bind();
        });
        m_webServerThread.detach();
    }

    void setActive(bool active) override {
        m_active = active;
    }

    void receiveResourcesDirectory([[maybe_unused]] const std::string& resourcesDirectory) override {
        m_resourcesDirectory = resourcesDirectory;
    }

    void receiveUserDataDirectory([[maybe_unused]] const std::string& userDataDirectory) override {
    }

    int webPort() const override {
        return m_webServerPort;
    }

    void render(ICanvas& canvas) const override {
        if(m_mouthProvider) {
            // draw background
            canvas.fill(127, 127, 127);

            // draw based on mouth open proportion
            const double mouth_open_proportion = m_mouthProvider->proportion();
            const uint8_t value = std::floor(std::lerp(0.0, 255.0, mouth_open_proportion));
            canvas.drawPolygon({{64, 0}, {64 + 32, 0}, {64 + 32, 32}}, 0, value, value, true);
            canvas.drawEllipse(0, 0, 32, 32, 0, value, 0, true);
            canvas.drawLine(32, 0, 64, 32, value, 0, 0);
            canvas.drawLine(32, 32, 64 + 32, 0, 0, 0, value);
            // Imagine a circle at the right-most side of the canvas.
            // Draw a line from the center to the circle based on the mouth open proportion.
            const double angle = std::lerp(0.0, 2*M_PI, mouth_open_proportion);
            const double radius = 13;
            const double x = 64 + 32 + 16 + radius * std::cos(angle);
            const double y = 16 + radius * std::sin(angle);
            canvas.drawLine(64 + 32 + 16, 16, x, y, 0, value, 0);
            // draw outline of circle
            canvas.drawEllipse(64 + 32, 0, 32, 32, 0, 0, value, false);
        } else {
            canvas.fill(127, 0, 0);
        }
    }

    float framerate() const override {
        return 30;
    }

    void receiveDeviceResolution(const Resolution& resolution) override {
        m_deviceResolution = resolution;
    }

    std::vector<Resolution> supportedResolutions() const override {
        return {m_deviceResolution};
    }

    void setMouthProportionProvider(std::shared_ptr<IProportionProvider> provider) {
        m_mouthProvider = provider;
    }

    std::shared_ptr<attributes::IAttributeStore> getAttributeStore() {
        return m_attributes;
    }

private:
    Resolution m_deviceResolution;
    std::shared_ptr<StandardAttributeStore> m_attributes;
    std::shared_ptr<IProportionProvider> m_mouthProvider;
    bool m_active;
    bool m_initialized;
    std::thread m_webServerThread;
    int m_webServerPort;
    std::string m_resourcesDirectory;
};

// Interface to create and destroy you app.
// This is how your app is created and consumed as a library.
extern "C" IProtogenApp * create_app() {
    return new ProtogenAppTest();
}

extern "C" void destroy_app(IProtogenApp * app) {
    delete app;
}