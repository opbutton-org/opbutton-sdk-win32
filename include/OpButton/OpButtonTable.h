#pragma once

#include "OpButton/OpButtonWidget.h"

namespace OpButton
{
    class OpButtonTable : public OpButtonWidget
    {
    public:
        OpButtonTable( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonTable() override;

        OpButtonTable & columns( const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>> & _cols );
        OpButtonTable & rows( const OpButtonVector<OpButtonJson> & _rowData );
    };

} // namespace OpButton
