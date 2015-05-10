/*
 * Joystick.h
 *
 *  Created on: 2015-2-1
 *      Author: yong
 *
 *  Modified on: 2015-5-6
 *      Poster: rrrfff, http://blog.csdn.net/rrrfff
 */
#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "cocos2d.h"
using namespace cocos2d;

class JoystickEvent : public Ref {
public:
	virtual bool init(){
		return true;
	}
	CREATE_FUNC(JoystickEvent);

public:
	double mAnagle;

public:
	static const std::string getJoyStickEventString(){
		static const std::string __JOYSTICK__EVENT = "joystick_event";
		return __JOYSTICK__EVENT;
	}
};

class Joystick : public Layer {
private:
	Sprite *mJsBg;
	Sprite *mJsCenter;
	Joystick(){};

public:
	virtual ~Joystick(){
		// 取消事件监听
		_eventDispatcher->removeEventListenersForTarget(this);
	}
	virtual bool init(){
		bool result = false;
		do {
			// 父类初始化
			if (!Layer::init()) {
				break;
			}
			// joystick的背景
			this->mJsBg = Sprite::create("joystick_bg.png");
			if (!mJsBg) {
				break;
			}
			this->addChild(mJsBg);

			// joystick的中心点
			this->mJsCenter = Sprite::create("joystick_center.png");
			if (!this->mJsCenter) {
				break;
			}
			this->addChild(mJsCenter);

			// touch event监听
			auto touchListener = EventListenerTouchOneByOne::create();
			if (!touchListener) {
				break;
			}
			touchListener->setSwallowTouches(true);
			touchListener->onTouchBegan =
				CC_CALLBACK_2(Joystick::onTouchBegan, this);
			touchListener->onTouchMoved =
				CC_CALLBACK_2(Joystick::onTouchMoved, this);
			touchListener->onTouchEnded =
				CC_CALLBACK_2(Joystick::onTouchEnded, this);
			_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener,
																	 this);

			result = true;
		} while (0);

		return result;
	}
	CREATE_FUNC(Joystick);

protected:
	bool onTouchBegan(Touch *touch, Event *unused_event){
		auto centerArea = mJsCenter->getBoundingBox();
		centerArea.origin += this->getPosition();
		// 若触摸点在joystick的中心点，则继续接受事件
		return centerArea.containsPoint(touch->getLocation());
	}
	void onTouchMoved(Touch *touch, Event *unused_event)
	{
		Vec2 touchPoint = touch->getLocation();
		Vec2 centerPoint = this->getPosition();
		double x = touchPoint.x - centerPoint.x;
		double y = touchPoint.y - centerPoint.y;

		// 更新joystick中心点位置, 目的是想让中心点始终在它的背景图范围
		// joystick背景图半径
		double jsBgRadis = mJsBg->getContentSize().width * 0.5;
		//触摸点到中心点的实际距离
		double distanceOfTouchPointToCenter = sqrt(
			pow(centerPoint.x - touchPoint.x, 2) + pow(centerPoint.y - touchPoint.y, 2));
		if (distanceOfTouchPointToCenter >= jsBgRadis) {
			//利用等比关系计算delta x y
			mJsCenter->setPosition(x * (jsBgRadis / distanceOfTouchPointToCenter),
								   y * (jsBgRadis / distanceOfTouchPointToCenter));
		} else {
			mJsCenter->setPosition(touchPoint - centerPoint);
		}

		// 分发joystick event
		auto jsEvent = JoystickEvent::create();
		// 获得角度，
		//  第一象限是0, 90度
		//  第二象限是90, 180度
		//  第三象限是-90, -180度
		//  第四象限是-90, 0度
		jsEvent->mAnagle = atan2(y, x) * 180 / 3.1415926;
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(
			JoystickEvent::getJoyStickEventString(), jsEvent);
	}
	void onTouchEnded(Touch *touch, Event *unused_event){
		// 事件结束，还原joystick中心点位置
		mJsCenter->setPosition(0, 0);
	}
};

#endif /* __JOYSTICK_H__ */
