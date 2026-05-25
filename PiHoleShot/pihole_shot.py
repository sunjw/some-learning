import argparse
import datetime
import os

import comm_util
import log_util
from playwright.sync_api import TimeoutError as PlaywrightTimeoutError
from playwright.sync_api import sync_playwright

logger = log_util.logger

SHOT_DIR = 'shot'
DEFAULT_SCREENSHOT_PREFIX = 'pihole'


def get_chrome_path():
    chrome_candidates = []
    if comm_util.is_linux():
        chrome_candidates = [
            '/usr/bin/google-chrome',
            '/usr/bin/google-chrome-stable',
            '/usr/bin/google-chrome-beta',
            '/opt/google/chrome/chrome',
            '/opt/google/chrome-beta/chrome',
            '/snap/bin/chromium',
        ]
    elif comm_util.is_macos():
        chrome_candidates = [
            '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome',
            '/Applications/Google Chrome Beta.app/Contents/MacOS/Google Chrome Beta',
        ]
    elif comm_util.is_windows():
        local_app_data = os.environ.get('LOCALAPPDATA', '')
        program_files = os.environ.get('PROGRAMFILES', 'C:/Program Files')
        program_files_x86 = os.environ.get('PROGRAMFILES(X86)', 'C:/Program Files (x86)')
        chrome_candidates = [
            os.path.join(program_files, 'Google/Chrome/Application/chrome.exe'),
            os.path.join(program_files, 'Google/Chrome Beta/Application/chrome.exe'),
            os.path.join(program_files_x86, 'Google/Chrome/Application/chrome.exe'),
            os.path.join(program_files_x86, 'Google/Chrome Beta/Application/chrome.exe'),
            os.path.join(local_app_data, 'Google/Chrome/Application/chrome.exe'),
            os.path.join(local_app_data, 'Google/Chrome Beta/Application/chrome.exe'),
        ]

    for chrome_path in chrome_candidates:
        if os.path.isfile(chrome_path):
            return chrome_path

    return None


def build_screenshot_path():
    shot_dir_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), SHOT_DIR)
    if not os.path.isdir(shot_dir_path):
        logger.info('Make directory [%s]', shot_dir_path)
        os.makedirs(shot_dir_path, exist_ok=True)

    timestamp = datetime.datetime.now().strftime('%y%m%d%H%M%S')
    file_name = '%s-%s.png' % (DEFAULT_SCREENSHOT_PREFIX, timestamp)
    return os.path.join(shot_dir_path, file_name)


def wait_page_ready(page):
    try:
        page.wait_for_load_state('networkidle', timeout=5000)
    except PlaywrightTimeoutError:
        logger.debug('Wait for network idle timed out, continue with current page state.')
    page.wait_for_timeout(1000)


def is_login_page(page):
    password_selectors = [
        'input[type="password"]',
        'input[name="pw"]',
        'input[name="password"]',
        'input#current-password',
    ]
    for selector in password_selectors:
        password_input = page.locator(selector)
        if password_input.count() > 0 and password_input.first.is_visible():
            logger.info('Detected login page by selector [%s]', selector)
            return True

    return False


def is_dashboard_page(page):
    dashboard_selectors = [
        'a[href="index.php"]:has-text("Dashboard")',
        'text="Total queries"',
        'text="Queries Blocked"',
    ]
    for selector in dashboard_selectors:
        dashboard_element = page.locator(selector)
        if dashboard_element.count() > 0 and dashboard_element.first.is_visible():
            logger.info('Detected dashboard page by selector [%s]', selector)
            return True

    return False


def login_if_needed(page, pihole_password):
    if not is_login_page(page):
        logger.info('Login page not detected, continue to screenshot.')
        return False

    password_selectors = [
        'input[type="password"]',
        'input[name="pw"]',
        'input[name="password"]',
        'input#current-password',
    ]
    password_input = None
    for selector in password_selectors:
        locator = page.locator(selector)
        if locator.count() > 0 and locator.first.is_visible():
            password_input = locator.first
            break

    if password_input is None:
        raise RuntimeError('Login page detected but password input was not found.')

    logger.info('Fill Pi-hole password.')
    password_input.fill(pihole_password)

    remember_me_checkbox = page.locator('input[name="persistentlogin"], input#logincookie')
    remember_me_label = page.locator('label:has-text("Remember me for 7 days")')
    if remember_me_checkbox.count() > 0 and remember_me_checkbox.first.is_visible():
        if not remember_me_checkbox.first.is_checked():
            logger.info('Enable persistent login option.')
            if remember_me_label.count() > 0 and remember_me_label.first.is_visible():
                remember_me_label.first.click()

            if not remember_me_checkbox.first.is_checked():
                remember_me_checkbox.first.evaluate(
                    """node => {
                        node.checked = true;
                        node.dispatchEvent(new Event('input', { bubbles: true }));
                        node.dispatchEvent(new Event('change', { bubbles: true }));
                    }"""
                )

            if not remember_me_checkbox.first.is_checked():
                raise RuntimeError('Remember me checkbox was found but could not be enabled.')

    submit_selectors = [
        'button[type="submit"]',
        'input[type="submit"]',
        'button:has-text("Log in")',
        'button:has-text("Login")',
    ]
    submitted = False
    for selector in submit_selectors:
        submit_button = page.locator(selector)
        if submit_button.count() > 0 and submit_button.first.is_visible():
            logger.info('Submit login form by selector [%s]', selector)
            submit_button.first.click()
            submitted = True
            break

    if not submitted:
        logger.info('Submit login form by pressing Enter in password field.')
        password_input.press('Enter')

    wait_page_ready(page)
    return True


def capture_pihole_admin_page(pihole_url, pihole_password):
    chrome_path = get_chrome_path()
    if chrome_path is None:
        raise RuntimeError('Google Chrome was not found on this machine.')

    screenshot_path = build_screenshot_path()
    logger.info('Use Chrome [%s]', chrome_path)
    logger.info('Open [%s]', pihole_url)

    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(
            executable_path=chrome_path,
            headless=True,
            args=['--no-sandbox', '--disable-dev-shm-usage'],
        )
        context = browser.new_context(
            ignore_https_errors=True,
            viewport={'width': 1440, 'height': 2000},
        )
        page = context.new_page()

        page.goto(pihole_url, wait_until='domcontentloaded')
        wait_page_ready(page)
        login_if_needed(page, pihole_password)

        if not is_dashboard_page(page):
            raise RuntimeError(
                'Pi-hole dashboard was not detected before screenshot. Current URL: [%s], title: [%s]'
                % (page.url, page.title())
            )

        logger.info('Save full page screenshot to [%s]', screenshot_path)
        page.screenshot(path=screenshot_path, full_page=True)

        context.close()
        browser.close()

    return screenshot_path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', dest='pihole_url', required=True,
                        help='url of pihole, for example http://192.168.1.1/pihole/admin/')
    parser.add_argument('--pswd', dest='pihole_password', required=True,
                        help='password for pihole')

    args = vars(parser.parse_args())
    # logger.debug('args\n%s', comm_util.pprint_dict_to_string(args))

    logger.info('PiHoleShot go!')

    pihole_url = args['pihole_url']
    pihole_password = args['pihole_password']

    screenshot_path = capture_pihole_admin_page(pihole_url, pihole_password)
    logger.info('Screenshot saved: [%s]', screenshot_path)


if __name__ == '__main__':
    main()
