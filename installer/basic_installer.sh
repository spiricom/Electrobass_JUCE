#!/bin/bash
export MAC_SIGNING_CERT=""
export MAC_INSTALLING_CERT=""
export MAC_SIGNING_ID=""
export MAC_SIGNING_TEAM=""
export MAC_SIGNING_1UPW=""

# Documentation for pkgbuild and productbuild: https://developer.apple.com/library/archive/documentation/DeveloperTools/Reference/DistributionDefinitionRef/Chapters/Distribution_XML_Ref.html

# preflight check
/Users/dp9443/Documents/Princeton/Electrobass_JUCE/Electrobass/Builds/MacOSX/build/Release
INDIR="Electrobass/Builds/MacOSX/build/Release"
SOURCEDIR="Resources"
TARGET_DIR="target"
VERSION=$1

TMPDIR="./installer-tmp"
mkdir -p $TMPDIR

echo "MAKE from $INDIR $SOURCEDIR into $TARGET_DIR with $VERSION"

VST3="Electrobass.vst3"
AU="Electrobass.component"
APP="Electrobass.app"

if [ "$VERSION" == "" ]; then
    echo "You must specify the version you are packaging!"
    echo "eg: ./make_installer.sh 1.0.6b4"
    exit 1
fi


OUTPUT_BASE_FILENAME="Electrobass$VERSION"

build_flavor()
{
    flavor=$1
    flavorprod=$2
    ident=$3
    loc=$4

    echo --- BUILDING Electrobass_${flavor}.pkg from "$flavorprod" ---

    workdir=$TMPDIR/$flavor
    mkdir -p $workdir

    # In the past we would pkgbuild --analyze first to make a plist file, but we are perfectly fine with the
    # defaults, so we skip that step here. http://thegreyblog.blogspot.com/2014/06/os-x-creating-packages-from-command_2.html
    # was pretty handy in figuring that out and man pkgbuild convinced us to not do it, as did testing.
    #
    # The defaults only work if a component is a sole entry in a staging directory though, so synthesize that
    # by moving the product to a tmp dir

    cp -r "$INDIR/$flavorprod" "$workdir"
    ls -l $workdir


    if [[ ! -z $MAC_SIGNING_CERT ]]; then
      [[ -z $MAC_INSTALLING_CERT ]] && echo "You need an installing cert too " && exit 2
      codesign --force -s "$MAC_SIGNING_CERT" -o runtime --deep "$workdir/$flavorprod"
      codesign -vvv "$workdir/$flavorprod"

      pkgbuild --sign "$MAC_INSTALLING_CERT" --root $workdir --identifier $ident --version $VERSION --install-location "$loc" "$TMPDIR/Electrobass_${flavor}.pkg" || exit 1
    else
      pkgbuild --root $workdir --identifier $ident --version $VERSION --install-location "$loc" "$TMPDIR/Electrobass_${flavor}.pkg" || exit 1
    fi

    rm -rf $workdir
}


# try to build VST3 package

if [[ -d $INDIR/$VST3 ]]; then
    build_flavor "VST3" "$VST3" "Electrobass.vst3.pkg" "/Library/Audio/Plug-Ins/VST3"
else 
    echo "could not find " $INDIR/$VST3 
fi


if [[ -d $INDIR/$AU ]]; then
    build_flavor "AU" "$AU" "Electrobass.component.pkg" "/Library/Audio/Plug-Ins/Components"
fi


if [[ -d $INDIR/$APP ]]; then
    build_flavor "APP" "$APP" "Electrobass.app.pkg" "/tmp/bkApp"
fi
# Build the resources pagkage
RSRCS=${SOURCEDIR}
echo --- BUILDING Resources pkg ---
if [[ ! -z $MAC_INSTALLING_CERT ]]; then
  pkgbuild --sign "$MAC_INSTALLING_CERT" --root "$RSRCS" --identifier "Electrobass.resources.pkg" --version $VERSION --scripts ResourcesPackageScript --install-location "/tmp/Electrobass" ${TMPDIR}/Electrobass_Resources.pkg
else
  pkgbuild --root "$RSRCS" --identifier "Electrobass.resources.pkg" --version $VERSION --scripts ResourcesPackageScript --install-location "/tmp/Electrobass" ${TMPDIR}/Electrobass_Resources.pkg
fi

echo --- Sub Packages Created ---
ls -l "${TMPDIR}"

# create distribution.xml

if [[ -d $INDIR/$VST3 ]]; then
	VST3_PKG_REF='<pkg-ref id="Electrobass.vst3.pkg"/>'
	VST3_CHOICE='<line choice="Electrobass.vst3.pkg"/>'
	VST3_CHOICE_DEF="<choice id=\"Electrobass.vst3.pkg\" visible=\"true\" start_selected=\"true\" title=\"Electrobass VST\"><pkg-ref id=\"Electrobass.vst3.pkg\"/></choice><pkg-ref id=\"Electrobass.vst3.pkg\" version=\"${VERSION}\" onConclusion=\"none\">Electrobass_VST3.pkg</pkg-ref>"
fi
if [[ -d $INDIR/$AU ]]; then
	AU_PKG_REF='<pkg-ref id="Electrobass.component.pkg"/>'
	AU_CHOICE='<line choice="Electrobass.component.pkg"/>'
	AU_CHOICE_DEF="<choice id=\"Electrobass.component.pkg\" visible=\"true\" start_selected=\"true\" title=\"Electrobass Audio Unit\"><pkg-ref id=\"Electrobass.component.pkg\"/></choice><pkg-ref id=\"Electrobass.component.pkg\" version=\"${VERSION}\" onConclusion=\"none\">Electrobass_AU.pkg</pkg-ref>"
fi
if [[ -d $INDIR/$APP ]]; then
	APP_PKG_REF='<pkg-ref id="Electrobass.app.pkg"/>'
	APP_CHOICE='<line choice="Electrobass.app.pkg"/>'
	APP_CHOICE_DEF="<choice id=\"Electrobass.app.pkg\" visible=\"true\" start_selected=\"true\" title=\"Electrobass App\"><pkg-ref id=\"Electrobass.app.pkg\"/></choice><pkg-ref id=\"Electrobass.app.pkg\" version=\"${VERSION}\" onConclusion=\"none\">Electrobass_APP.pkg</pkg-ref>"
fi

cat > $TMPDIR/distribution.xml << XMLEND
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>Electrobass ${VERSION}</title>
    <license file="License.txt" />
    <readme file="Readme.rtf" />
    ${VST3_PKG_REF}
    ${AU_PKG_REF}
    ${APP_PKG_REF}
    <pkg-ref id="Electrobass.resources.pkg"/>
    <options require-scripts="false" customize="always" />
    <choices-outline>
        ${VST3_CHOICE}
        ${AU_CHOICE}
        ${APP_CHOICE}
        <line choice="Electrobass.resources.pkg"/>
    </choices-outline>
    ${VST3_CHOICE_DEF}
    ${AU_CHOICE_DEF}
    ${APP_CHOICE_DEF}
    <choice id="Electrobass.resources.pkg" visible="true" enabled="false" selected="true" title="Install resources">
        <pkg-ref id="Electrobass.resources.pkg"/>
    </choice>
    <pkg-ref id="Electrobass.resources.pkg" version="${VERSION}" onConclusion="none">Electrobass_Resources.pkg</pkg-ref>
</installer-gui-script>
XMLEND

# build installation bundle

pushd ${TMPDIR}
if [[ ! -z $MAC_INSTALLING_CERT ]]; then
  echo "Building SIGNED PKG"
  productbuild --sign "$MAC_INSTALLING_CERT" --distribution "distribution.xml" --package-path "." --resources ${SOURCEDIR}/scripts/installer_mac/Resources "$OUTPUT_BASE_FILENAME.pkg"
else
  echo "Building UNSIGNED PKG"
  productbuild --distribution "distribution.xml" --package-path "." --resources ${SOURCEDIR}/scripts/installer_mac/Resources "$OUTPUT_BASE_FILENAME.pkg"
fi

popd

Rez -append ${SOURCEDIR}/icns.rsrc -o "${TMPDIR}/${OUTPUT_BASE_FILENAME}.pkg"
SetFile -a C "${TMPDIR}/${OUTPUT_BASE_FILENAME}.pkg"
mkdir "${TMPDIR}/Electrobass"
mv "${TMPDIR}/${OUTPUT_BASE_FILENAME}.pkg" "${TMPDIR}/Electrobass"
# create a DMG if required

if [[ -f "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg" ]]; then
  rm "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg"
fi
hdiutil create /tmp/tmp.dmg -ov -volname "$OUTPUT_BASE_FILENAME" -fs HFS+ -srcfolder "${TMPDIR}/Electrobass/"
hdiutil convert /tmp/tmp.dmg -format UDZO -o "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg"

if [[ ! -z $MAC_SIGNING_CERT ]]; then
  codesign --force -s "$MAC_SIGNING_CERT" --timestamp "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg"
  codesign -vvv "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg"
  # if I have xcode 13
  # xcrun notarytool submit "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg" --apple-id ${MAC_SIGNING_ID} --team-id ${MAC_SIGNING_TEAM} --password ${MAC_SIGNING_1UPW} --wait

  # but if i dont
  ruuid=$(xcrun altool --notarize-app --primary-bundle-id "Electrobass" \
              --username ${MAC_SIGNING_ID} --password ${MAC_SIGNING_1UPW} --asc-provider ${MAC_SIGNING_TEAM} \
              --file "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg" 2>&1 | tee altool.out \
             | awk '/RequestUUID/ { print $NF; }')
  echo "REQUEST UID : $ruuid"

  if [[ $ruuid == "" ]]; then
        echo "could not upload for notarization"
        cat altool.out
        exit 1
  fi

  request_status="in progress"
  while [[ "$request_status" == "in progress" ]]; do
      echo -n "waiting... "
      sleep 10
      xcrun altool --notarization-info "$ruuid" \
                                      --username "${MAC_SIGNING_ID}" \
                                      --password "${MAC_SIGNING_1UPW}" \
                                      --asc-provider "${MAC_SIGNING_TEAM}" 

      request_status=$(xcrun altool --notarization-info "$ruuid" \
                                      --username "${MAC_SIGNING_ID}" \
                                      --password "${MAC_SIGNING_1UPW}" \
                                      --asc-provider "${MAC_SIGNING_TEAM}" 2>&1 \
                         | awk -F ': ' '/Status:/ { print $2; }' )
      echo "$request_status"
  done
  xcrun stapler staple "${TARGET_DIR}/${OUTPUT_BASE_FILENAME}.dmg"
fi

# clean up

#rm distribution.xml
#rm Electrobass_*.pkg
