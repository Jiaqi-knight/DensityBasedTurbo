/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Author
    Oliver Borm  All rights reserved.

\*---------------------------------------------------------------------------*/

#include "isentropicTotalTemperatureFvPatchScalarField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"

#include "basicThermo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

isentropicTotalTemperatureFvPatchScalarField::isentropicTotalTemperatureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(p, iF),
    pName_("Undefined"),
    T0_(p.size(), 0.0),
    p0_(p.size(), 0.0)
{}


isentropicTotalTemperatureFvPatchScalarField::isentropicTotalTemperatureFvPatchScalarField
(
    const isentropicTotalTemperatureFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchScalarField(ptf, p, iF, mapper),
    pName_(ptf.pName_),
    T0_(ptf.T0_, mapper),
    p0_(ptf.p0_, mapper)
{}


isentropicTotalTemperatureFvPatchScalarField::isentropicTotalTemperatureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchScalarField(p, iF),
    pName_(dict.lookup("p")),
    T0_("T0", dict, p.size()),
    p0_("p0", dict, p.size())
{
    if (dict.found("value"))
    {
        fvPatchField<scalar>::operator=
        (
            scalarField("value", dict, p.size())
        );
    }
    else
    {
        fvPatchField<scalar>::operator=(T0_);
    }
}


isentropicTotalTemperatureFvPatchScalarField::isentropicTotalTemperatureFvPatchScalarField
(
    const isentropicTotalTemperatureFvPatchScalarField& tppsf
)
:
    fixedValueFvPatchScalarField(tppsf),
    pName_(tppsf.pName_),
    T0_(tppsf.T0_),
    p0_(tppsf.p0_)
{}


isentropicTotalTemperatureFvPatchScalarField::isentropicTotalTemperatureFvPatchScalarField
(
    const isentropicTotalTemperatureFvPatchScalarField& tppsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(tppsf, iF),
    pName_(tppsf.pName_),
    T0_(tppsf.T0_),
    p0_(tppsf.p0_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void isentropicTotalTemperatureFvPatchScalarField::autoMap
(
    const fvPatchFieldMapper& m
)
{
    fixedValueFvPatchScalarField::autoMap(m);
    T0_.autoMap(m);
    p0_.autoMap(m);
}


void isentropicTotalTemperatureFvPatchScalarField::rmap
(
    const fvPatchScalarField& ptf,
    const labelList& addr
)
{
    fixedValueFvPatchScalarField::rmap(ptf, addr);

    const isentropicTotalTemperatureFvPatchScalarField& tiptf =
        refCast<const isentropicTotalTemperatureFvPatchScalarField>(ptf);

    T0_.rmap(tiptf.T0_, addr);
    p0_.rmap(tiptf.p0_, addr);
}


void isentropicTotalTemperatureFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }
    const fvPatchField<scalar>& p =
        patch().lookupPatchField<volScalarField, scalar>(pName_);

    const basicThermo& thermo =
        db().lookupObject<basicThermo>("thermophysicalProperties");

    volScalarField gamma = thermo.Cp()/thermo.Cv();

    const fvPatchField<scalar>& gammap =
        patch().patchField<volScalarField, scalar>(gamma);

    scalarField gM1ByG = (gammap - 1.0)/gammap;

    operator==(T0_*pow(p/p0_,gM1ByG));

    fixedValueFvPatchScalarField::updateCoeffs();
}

void isentropicTotalTemperatureFvPatchScalarField::updateCoeffs(const vectorField& Up)
{
    updateCoeffs();
}


void isentropicTotalTemperatureFvPatchScalarField::write(Ostream& os) const
{
    fvPatchScalarField::write(os);
    os.writeKeyword("p") << pName_ << token::END_STATEMENT << nl;
    T0_.writeEntry("T0", os);
    p0_.writeEntry("p0", os);
    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField(fvPatchScalarField, isentropicTotalTemperatureFvPatchScalarField);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
