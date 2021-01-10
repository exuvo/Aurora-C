/*
 * StrategicIconComponent.hpp
 *
 *  Created on: Dec 18, 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_STRATEGICICONCOMPONENT_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_STRATEGICICONCOMPONENT_HPP_

//TODO implement
class TextureRegion;

// Icon shape/outline
struct StrategicIconBase {
	const char path[32];
	const uint64_t massLimit; // In nm
	
	bool operator==(const StrategicIconBase& o) const {
		return this == &o;
	}
};

struct StrategicIconsBase {
	static inline constexpr StrategicIconBase GIGANTIC { "strategic/gigantic", 1'000'000 };
	static inline constexpr StrategicIconBase MASSIVE { "strategic/massive", 300'000 };
	static inline constexpr StrategicIconBase HUGE { "strategic/huge", 125'000 };
	static inline constexpr StrategicIconBase LARGE { "strategic/large", 50'000 };
	static inline constexpr StrategicIconBase MEDIUM { "strategic/medium", 10'000 };
	static inline constexpr StrategicIconBase SMALL { "strategic/small", 2'000 };
	static inline constexpr StrategicIconBase TINY { "strategic/tiny", 200 };
	
	static inline constexpr StrategicIconBase BOMBER { "strategic/bomber", 30 };
	static inline constexpr StrategicIconBase FIGHTER { "strategic/fighter", 20 };
	
	static inline constexpr StrategicIconBase COLONY { "strategic/colony", 0 };
	static inline constexpr StrategicIconBase OUTPOST { "strategic/outpost", 0 };
	
	static inline constexpr StrategicIconBase STARBASE { "strategic/starbase", 0 };
	static inline constexpr StrategicIconBase FORTRESS { "strategic/fortress", 0 };
	
	static inline constexpr StrategicIconBase ASTEROID1 { "strategic/asteroid1", 0 };
	static inline constexpr StrategicIconBase ASTEROID2 { "strategic/asteroid2", 0 };
	static inline constexpr StrategicIconBase MINE { "strategic/mine", 0 };
	static inline constexpr StrategicIconBase NONE { "strategic/unknown1", 0 };
	
	static inline constexpr StrategicIconBase ALL[] { GIGANTIC, MASSIVE, HUGE, LARGE, MEDIUM,
	                                                  SMALL, TINY, BOMBER, FIGHTER, COLONY,
	                                                  OUTPOST, STARBASE, FORTRESS, ASTEROID1,
	                                                  ASTEROID2, MINE, NONE };
};

struct StrategicIconCenter {
	const char path[32];
	
	bool operator==(const StrategicIconCenter& o) const {
		return this == &o;
	}
};

struct StrategicIconsCenter {
	static inline constexpr StrategicIconCenter RAILGUN1 { "strategic/cRailgun1" };
	static inline constexpr StrategicIconCenter RAILGUN2 { "strategic/cRailgun2" };
	static inline constexpr StrategicIconCenter LASER1 { "strategic/cLaser1" };
	static inline constexpr StrategicIconCenter LASER2 { "strategic/cLaser2" };
	static inline constexpr StrategicIconCenter MISSILE1 { "strategic/cMissile1" };
	static inline constexpr StrategicIconCenter MISSILE2 { "strategic/cMissile2" };
	static inline constexpr StrategicIconCenter MISSILE3 { "strategic/cMissile3" };
	static inline constexpr StrategicIconCenter MISSILE4 { "strategic/cMissile4" };
	static inline constexpr StrategicIconCenter THREE { "strategic/cThree" };
	static inline constexpr StrategicIconCenter INTEL { "strategic/cIntel" };
	static inline constexpr StrategicIconCenter HEALING_CIRCLE { "strategic/cHealingCircle" };
	static inline constexpr StrategicIconCenter BRACKETS1 { "strategic/cBrackets1" };
	static inline constexpr StrategicIconCenter BRACKETS2 { "strategic/cBrackets2" };
	static inline constexpr StrategicIconCenter NONE { "strategic/cUnknown" };
	
	static inline constexpr StrategicIconCenter ALL[] { RAILGUN1, RAILGUN2, LASER1, LASER2, MISSILE1,
	                                                  MISSILE2, MISSILE3, MISSILE4, THREE, INTEL,
	                                                  HEALING_CIRCLE, BRACKETS1, BRACKETS2, NONE};
};

struct StrategicIcon {
	const StrategicIconBase* base = &StrategicIconsBase::NONE;
	const StrategicIconCenter* center = &StrategicIconsCenter::NONE;
};

struct StrategicIconComponent {
	TextureRegion* baseTexture;
	TextureRegion* centerTexture;
	
	StrategicIconComponent(TextureRegion* icon) {
		baseTexture = icon;
		centerTexture = nullptr;
	}
	
	StrategicIconComponent(TextureRegion* baseIcon, TextureRegion* centerIcon) {
		baseTexture = baseIcon;
		centerTexture = centerIcon;
	}
};



#endif /* SRC_STARSYSTEMS_COMPONENTS_STRATEGICICONCOMPONENT_HPP_ */
