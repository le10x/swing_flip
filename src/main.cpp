#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        int m_clickCount = 0;
        CCObject* m_lastButton = nullptr;
        std::chrono::system_clock::time_point m_lastClickTime;
    };

    // Corregido: Ahora acepta el argumento 'unfocused'
    bool init(bool unfocused) {
        if (!PauseLayer::init(unfocused)) return false;

        if (Mod::get()->getSettingValue<bool>("enable-opacity")) {
            this->applyOpacityToButtons(180); 
        }
        return true;
    }

    void applyOpacityToButtons(uint8_t opacity) {
        const char* buttons[] = {"exit-button", "restart-button", "practice-button"};
        for (const char* id : buttons) {
            if (auto btn = this->getChildByIDRecursive(id)) {
                // Corregido: Cast a CCRGBAProtocol para poder usar setOpacity
                if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(btn)) {
                    rgba->setOpacity(opacity);
                }
            }
        }
    }

    void handleSafeClick(CCObject* sender, std::string_view settingKey, std::function<void(CCObject*)> originalFunc) {
        if (!Mod::get()->getSettingValue<bool>(std::string(settingKey))) {
            originalFunc(sender);
            return;
        }

        auto playLayer = PlayLayer::get();
        if (playLayer && !playLayer->m_isPlatformer) {
            int64_t minPercent = Mod::get()->getSettingValue<int64_t>("min-percent");
            if (playLayer->getCurrentPercent() < static_cast<double>(minPercent)) {
                originalFunc(sender);
                return;
            }
        }

        auto ahora = std::chrono::system_clock::now();
        int64_t speedLimit = Mod::get()->getSettingValue<int64_t>("click-speed");
        auto tiempoTranscurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClickTime).count();

        if (m_fields->m_lastButton != sender || tiempoTranscurrido > speedLimit) {
            m_fields->m_clickCount = 0;
            this->removeChildByTag(69420);
            if (Mod::get()->getSettingValue<bool>("enable-opacity")) this->applyOpacityToButtons(180);
        }

        m_fields->m_clickCount++;
        m_fields->m_lastButton = sender;
        m_fields->m_lastClickTime = ahora;

        if (m_fields->m_clickCount >= 2) {
            m_fields->m_clickCount = 0;
            originalFunc(sender);
        } else {
            // Corregido: Cast para opacidad en el botón presionado
            if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(sender)) {
                rgba->setOpacity(255);
            }

            if (Mod::get()->getSettingValue<bool>("show-message")) {
                std::string texto = Mod::get()->getSettingValue<std::string>("custom-text");
                bool gold = Mod::get()->getSettingValue<bool>("use-gold-font");
                
                auto label = CCLabelBMFont::create(texto.c_str(), gold ? "goldFont.fnt" : "bigFont.fnt");
                auto winSize = CCDirector::get()->getWinSize();
                label->setPosition({winSize.width / 2, winSize.height / 2 - 60});
                label->setScale(0.5f);
                label->setTag(69420);
                
                this->removeChildByTag(69420);
                this->addChild(label);
                
                label->runAction(CCSequence::create(
                    CCEaseExponentialOut::create(CCScaleTo::create(0.2f, 0.6f)),
                    CCDelayTime::create(0.4f),
                    CCFadeOut::create(0.2f),
                    CCRemoveSelf::create(),
                    nullptr
                ));
            }
        }
    }

    void onQuit(CCObject* s) { handleSafeClick(s, "lock-exit", [this](CCObject* o) { PauseLayer::onQuit(o); }); }
    void onRestart(CCObject* s) { handleSafeClick(s, "lock-reset", [this](CCObject* o) { PauseLayer::onRestart(o); }); }
    void onRestartFull(CCObject* s) { handleSafeClick(s, "lock-reset-plat", [this](CCObject* o) { PauseLayer::onRestartFull(o); }); }
    void onPracticeMode(CCObject* s) { handleSafeClick(s, "lock-practice", [this](CCObject* o) { PauseLayer::onPracticeMode(o); }); }
    void onNormalMode(CCObject* s) { handleSafeClick(s, "lock-exit-practice", [this](CCObject* o) { PauseLayer::onNormalMode(o); }); }
};
