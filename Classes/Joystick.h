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
		// ȡ���¼�����
		_eventDispatcher->removeEventListenersForTarget(this);
	}
	virtual bool init(){
		bool result = false;
		do {
			// �����ʼ��
			if (!Layer::init()) {
				break;
			}
			// joystick�ı���
			this->mJsBg = Sprite::create("joystick_bg.png");
			if (!mJsBg) {
				break;
			}
			this->addChild(mJsBg);

			// joystick�����ĵ�
			this->mJsCenter = Sprite::create("joystick_center.png");
			if (!this->mJsCenter) {
				break;
			}
			this->addChild(mJsCenter);

			// touch event����
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
		// ����������joystick�����ĵ㣬����������¼�
		return centerArea.containsPoint(touch->getLocation());
	}
	void onTouchMoved(Touch *touch, Event *unused_event)
	{
		Vec2 touchPoint = touch->getLocation();
		Vec2 centerPoint = this->getPosition();
		double x = touchPoint.x - centerPoint.x;
		double y = touchPoint.y - centerPoint.y;

		// ����joystick���ĵ�λ��, Ŀ�����������ĵ�ʼ�������ı���ͼ��Χ
		// joystick����ͼ�뾶
		double jsBgRadis = mJsBg->getContentSize().width * 0.5;
		//�����㵽���ĵ��ʵ�ʾ���
		double distanceOfTouchPointToCenter = sqrt(
			pow(centerPoint.x - touchPoint.x, 2) + pow(centerPoint.y - touchPoint.y, 2));
		if (distanceOfTouchPointToCenter >= jsBgRadis) {
			//���õȱȹ�ϵ����delta x y
			mJsCenter->setPosition(x * (jsBgRadis / distanceOfTouchPointToCenter),
								   y * (jsBgRadis / distanceOfTouchPointToCenter));
		} else {
			mJsCenter->setPosition(touchPoint - centerPoint);
		}

		// �ַ�joystick event
		auto jsEvent = JoystickEvent::create();
		// ��ýǶȣ�
		//  ��һ������0, 90��
		//  �ڶ�������90, 180��
		//  ����������-90, -180��
		//  ����������-90, 0��
		jsEvent->mAnagle = atan2(y, x) * 180 / 3.1415926;
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(
			JoystickEvent::getJoyStickEventString(), jsEvent);
	}
	void onTouchEnded(Touch *touch, Event *unused_event){
		// �¼���������ԭjoystick���ĵ�λ��
		mJsCenter->setPosition(0, 0);
	}
};

#endif /* __JOYSTICK_H__ */
