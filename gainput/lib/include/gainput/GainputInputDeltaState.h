
#ifndef GAINPUTINPUTDELTASTATE_H_
#define GAINPUTINPUTDELTASTATE_H_

namespace gainput
{

/// Stores a list of gainput state changes.
class GAINPUT_LIBEXPORT InputDeltaState
{
public:
	InputDeltaState(Allocator& allocator);

	/// Add a state change for a bool-type button.
	/**
	 * \param device The gainput device the change occurred on.
	 * \param deviceButton The gainput button that was changed.
	 * \param oldValue The old button state.
	 * \param newValue The new button state.
	 */
	void AddChange(DeviceId device, DeviceButtonId deviceButton, bool oldValue, bool newValue);
	/// Add a state change for a float-type button.
	/**
	 * \param device The gainput device the change occurred on.
	 * \param deviceButton The gainput button that was changed.
	 * \param oldValue The old button state.
	 * \param newValue The new button state.
	 */
	void AddChange(DeviceId device, DeviceButtonId deviceButton, float oldValue, float newValue);

	/// Clear list of state changes.
	void Clear();

	/// Notifies all gainput listeners of the previously recorded state changes.
	/**
	 * \param listeners A list of gainput listeners to notify.
	 */
	void NotifyListeners(Array<InputListener*>& listeners) const;

private:
	struct Change
	{
		DeviceId device;
		DeviceButtonId deviceButton;
		ButtonType type;
		union
		{
			bool b;
			float f;
		} oldValue, newValue;
	};

	Array<Change> changes_;
};

}

#endif

