#include <Geode/Geode.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/binding/GJUserScore.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <Geode/modify/GameLevelManager.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

int attReq = 5000;
std::string kv = "vg";

bool vgCheck(GJGameLevel* lvl) {
	auto k = geode::Mod::get()->getSavedValue<std::vector<int>>(kv);

	return std::find(k.begin(), k.end(), lvl->m_levelID.value()) != k.end();
}

class $modify(VengeanceInfo, LevelInfoLayer) {
	bool init(GJGameLevel* lvl, bool c) {
		if (!LevelInfoLayer::init(lvl,c)) return false;

		if ((lvl->m_attempts >= attReq) && (lvl->m_levelType != GJLevelType::Editor)) {
            auto title = static_cast<CCLabelBMFont*>(this->getChildByIDRecursive("title-label"));

			if (title) {
				if (vgCheck(lvl)) {
					title->setColor({255,215,100});
				} else {
					title->setColor({255,0,0});
				}
			}

			if (!this->getChildByID("vg-particles")) {
				auto particles = CCParticleSystemQuad::create();
				if (particles && particles->initWithTotalParticles(50, true)) {
					particles->setID("vg-particles");

					auto texture = CCTextureCache::sharedTextureCache()->addImage("cc_2x2_white_image", false);

					if (texture) {
						particles->setTexture(texture);

						auto size = texture->getContentSize();
						particles->setTextureWithRect(texture, {0,0,size.width,size.height});
					}

					particles->setTotalParticles(50);
					particles->setEmissionRate(20.f);
					particles->setDuration(-1.f);
					particles->setLife(2.f);
					particles->setLifeVar(0.5f);
					particles->setGravity({0,-15});
					particles->setSpeed(17.5f);
					particles->setAngle(90.f);
					particles->setAngleVar(25.f);

					float tWidth = title->getContentSize().width;
					particles->setPosVar({tWidth/2, 5.f});

					if (vgCheck(lvl)) {
						particles->setStartColor({1.f,0.84f,0.f,0.5f});
						particles->setEndColor({1.f,0.84f,0.f,0.f});
					} else {
						particles->setStartColor({1.f,0.f,0.f,0.5f});
						particles->setEndColor({1.f,0.f,0.f,0.f});
					}

					
					particles->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
					particles->setStartSize(4.5f); 
					particles->setStartSizeVar(1.f);
					particles->setEndSize(2.f);
					particles->setEndSizeVar(1.f);


					particles->setZOrder(999);

					float tHeight = title->getContentSize().height;
					particles->setPosition(title->getPosition() - CCPoint{0, tHeight/4});

					this->addChild(particles);
				}
			}
        }

		return true;
	}
};

class $modify(VengeanceCell, LevelCell) {
	void loadFromLevel(GJGameLevel* lvl) {
        LevelCell::loadFromLevel(lvl);

		bool enabled = Mod::get()->getSavedValue<bool>("vg-filter", false);

        if (enabled && (lvl->m_attempts >= attReq) && (lvl->m_levelType != GJLevelType::Editor)) {
            auto title = static_cast<CCLabelBMFont*>(this->getChildByIDRecursive("level-name"));
            if (title) {
				if (vgCheck(lvl)) {
					title->setColor({255,215,100});
				} else {
					title->setColor({255,0,0});
				}
			}

			if (!this->getChildByID("vg-particles")) {
				auto particles = CCParticleSystemQuad::create();
				if (particles && particles->initWithTotalParticles(50, true)) {
					particles->setID("vg-particles");
					particles->setPositionType(cocos2d::kCCPositionTypeRelative);

					auto texture = CCTextureCache::sharedTextureCache()->addImage("cc_2x2_white_image", false);

					if (texture) {
						particles->setTexture(texture);

						auto size = texture->getContentSize();
						particles->setTextureWithRect(texture, {0,0,size.width,size.height});
					}

					particles->setTotalParticles(50);
					particles->setEmissionRate(20.f);
					particles->setDuration(-1.f);
					particles->setLife(2.f);
					particles->setLifeVar(0.5f);
					particles->setGravity({0,-10});
					particles->setSpeed(12.5f);
					particles->setAngle(90.f);
					particles->setAngleVar(25.f);

					float tWidth = title->getContentSize().width;
					float tHeight = title->getContentSize().height;
					particles->setPosVar({tWidth/2, tHeight/2});

					if (vgCheck(lvl)) {
						particles->setStartColor({1.f,0.84f,0.f,0.5f});
						particles->setEndColor({1.f,0.84f,0.f,0.f});
					} else {
						particles->setStartColor({1.f,0.f,0.f,0.5f});
						particles->setEndColor({1.f,0.f,0.f,0.f});
					}

					particles->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
					particles->setStartSize(4.5f); 
					particles->setStartSizeVar(1.f);
					particles->setEndSize(2.f);
					particles->setEndSizeVar(1.f);


					particles->setZOrder(999);
					
					float actualWidth = title->getContentSize().width * title->getScaleX();
					float actualHeight = title->getContentSize().height * title->getScaleY();
					float xOffset = (0.5f - title->getAnchorPoint().x) * actualWidth;
					float yOffset = (0.5f - title->getAnchorPoint().y) * actualHeight;
					particles->setPosition(title->getPosition() + CCPoint{ xOffset, yOffset });

					this->addChild(particles);
				}
			}
        }
    }
};

class $modify(GameLevelManager) {
	cocos2d::CCArray* getSavedLevels(bool fav, int fld) {
		auto ogLevels = GameLevelManager::getSavedLevels(fav, fld);

		if (Mod::get()->getSavedValue<bool>("vg-filter", false)) {
			auto vgLevels = cocos2d::CCArray::create();

			for (int i=0; i<ogLevels->count(); i++) {
				auto lvl = static_cast<GJGameLevel*>(ogLevels->objectAtIndex(i));

				if (lvl->m_attempts >= attReq) {
					vgLevels->addObject(lvl);
				}
			}

			return vgLevels;
		}

		return ogLevels;
	}
};

class $modify(VengeanceBrowserLayer, LevelBrowserLayer) {
	void vgToggle(cocos2d::CCObject* sender) {
		bool enabled = Mod::get()->getSavedValue<bool>("vg-filter", false);
		Mod::get()->setSavedValue("vg-filter", !enabled);

		auto scene = LevelBrowserLayer::scene(this->m_searchObject);
		cocos2d::CCDirector::sharedDirector()->replaceScene(scene);
	}

	bool init(GJSearchObject* searchObj) {
		if (!LevelBrowserLayer::init(searchObj)) return false;

		if (searchObj->m_searchType != SearchType::SavedLevels) return true;

		auto offSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        offSprite->setColor({255, 50, 50});
        onSprite->setColor({255, 50, 50});

		auto toggler = CCMenuItemToggler::create(
            offSprite,
            onSprite,
            this,
            menu_selector(VengeanceBrowserLayer::vgToggle)
        );

		bool enabled = Mod::get()->getSavedValue<bool>("vg-filter", false);
        toggler->toggle(enabled);

		if (auto favBtn = this->getChildByIDRecursive("favorite-button")) {

			if (auto parentMenu = typeinfo_cast<cocos2d::CCMenu*>(favBtn->getParent())) {
                parentMenu->addChild(toggler);

				toggler->setScale(0.6f);
                toggler->setPosition({
                    favBtn->getPositionX(),
                    favBtn->getPositionY() + 40.0f
                });
            }
		}

		return true;
	}

	void onBack(cocos2d::CCObject* sender) {
		LevelBrowserLayer::onBack(sender);
		
		if (this->m_searchObject && (this->m_searchObject->m_searchType == SearchType::SavedLevels)) {
			Mod::get()->setSavedValue("vg-filter", false);
		}
	}

	void onFavorites(cocos2d::CCObject* sender) {
		Mod::get()->setSavedValue("vg-filter", false);
		LevelBrowserLayer::onFavorites(sender);
	}
};

class $modify(pLayer, PlayLayer) {
	void levelComplete() {
		PlayLayer::levelComplete();

		auto lvl = m_level;
		if (!lvl) return;

		if (m_isPracticeMode || m_isTestMode || (m_startPosObject != nullptr) || (lvl->m_newNormalPercent2 < 100) || (lvl->m_normalPercent < 100)) return;

		if (lvl->m_attempts >= attReq) {
			auto k = geode::Mod::get()->getSavedValue<std::vector<int>>(kv);
			int id = lvl->m_levelID.value();
			if (std::find(k.begin(), k.end(), id) == k.end()) {
				k.push_back(id);
				geode::Mod::get()->setSavedValue(kv, k);
			}
		}
	}
};

class $modify(VengeanceProfile, ProfilePage) {
	void loadPageFromUserInfo(GJUserScore* score) {
		ProfilePage::loadPageFromUserInfo(score);

		auto accManager = GJAccountManager::sharedState();
		if (accManager->m_accountID != score->m_accountID) return;

		auto data = geode::Mod::get()->getSavedValue<std::vector<int>>(kv);
		int vgPts = data.size();

		auto userLabel = static_cast<CCLabelBMFont*>(this->getChildByIDRecursive("username-label"));

		if (userLabel) {
			if (auto mainLayer = this->m_mainLayer) {
				std::string txt = "Claimed Bounties: " + std::to_string(vgPts);
				auto vgLabel = CCLabelBMFont::create(txt.c_str(), "bigFont.fnt");
				
				vgLabel->setID("vg-pts-label");
				vgLabel->setScale(0.45f);
				vgLabel->setColor({255,50,50});
				
				auto wp = userLabel->getParent()->convertToWorldSpace(userLabel->getPosition());
				auto lp = this->convertToNodeSpace(wp);
				
				vgLabel->setPosition({
					lp.x,
					lp.y - 20.f
				});
			
				mainLayer->addChild(vgLabel);
			}
		}
	}
};