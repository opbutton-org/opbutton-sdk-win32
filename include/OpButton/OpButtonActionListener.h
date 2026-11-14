#pragma once

namespace OpButton {

class OpButtonAction;

class OpButtonActionListener {
public:
    virtual ~OpButtonActionListener() = default;
    virtual void onOpButtonActionReceived(const OpButtonAction& action) = 0;
};

} // namespace OpButton
