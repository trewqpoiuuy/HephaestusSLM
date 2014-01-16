from __future__ import absolute_import
import __init__
from fabmetheus_utilities import settings
from skeinforge_application.skeinforge_utilities import skeinforge_profile
import sys


__author__ = 'Ben Knutson'
__date__ = '$Date: 17.10.13 $'
__license__ = 'GNU Affero General Public License http://www.gnu.org/licenses/agpl.html'


def getCraftSequence():
	'Get the SLM craft sequence.'
	return 'carve scale bottom preface widen inset fill multiply raft skirt jitter clip smooth stretch cool  home lash fillet alteration export'.split()

def getNewRepository():
	'Get new repository.'
	return SLMRepository()


class SLMRepository:
	'A class to handle the export settings.'
	def __init__(self):
		'Set the default settings, execute title & settings fileName.'
		skeinforge_profile.addListsSetCraftProfile( getCraftSequence(), 'ABS', self, 'skeinforge_application.skeinforge_plugins.profile_plugins.extrusion.html')


def main():
	'Display the export dialog.'
	if len(sys.argv) > 1:
		writeOutput(' '.join(sys.argv[1 :]))
	else:
		settings.startMainLoopFromConstructor(getNewRepository())

if __name__ == '__main__':
	main()
