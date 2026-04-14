#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
    void updatePlayerFrame(int frame) {
        // Llamamos al original
        PlayerObject::updatePlayerFrame(frame);

        // Si es el Swing, tomamos el control visual
        if (this->m_isSwing) {
            // En lugar de setFlipY en el sprite, escalamos el Layer principal
            // Esto invierte TODO el conjunto (icono, ojos, glow) a la vez
            if (auto layer = this->m_mainLayer) {
                if (this->m_isUpsideDown) {
                    layer->setScaleY(-1.0f);
                } else {
                    layer->setScaleY(1.0f);
                }
            }
            
            // Importante: Si el icono se duplica, es porque m_iconSprite 
            // está intentando dibujarse por separado. Lo reseteamos aquí:
            if (this->m_iconSprite) {
                this->m_iconSprite->setFlipY(false); 
            }
        }
    }
};
